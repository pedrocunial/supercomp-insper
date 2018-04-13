#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <mpi.h>

#include "image.h"

#define MASTER 0
#define MATRIX 666

#define MAX(y,x) (y>x?y:x)    // Calcula valor maximo
#define MIN(y,x) (y<x?y:x)    // Calcula valor minimo

// Filtro de bordas
void edgeFilter(int *in, int *out, int rowStart, int rowEnd, int colStart, int colEnd)
{
  int i,j,di,dj;
  for(i = rowStart; i < rowEnd; ++i) {
    for(j = colStart; j < colEnd; ++j) {
      int min = 256;
      int max = 0;
      for(di = MAX(rowStart, i - 1); di <= MIN(i + 1, rowEnd - 1); di++) {
        for(dj = MAX(colStart, j - 1); dj <= MIN(j + 1, colEnd - 1); dj++) {
          if(min>in[di*(colEnd-colStart)+dj]) min = in[di*(colEnd-colStart)+dj];
          if(max<in[di*(colEnd-colStart)+dj]) max = in[di*(colEnd-colStart)+dj];
        }
      }
      out[i*(colEnd-colStart)+j] = max-min;
    }
  }
}


int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  int rank, size;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == MASTER) {
    // Estruturas que organizam as imagens PGM
    PGMData *imagemIn = malloc(sizeof(PGMData));
    PGMData *imagemOut = malloc(sizeof(PGMData));
    MPI_Request *request = malloc(sizeof(MPI_Request) * (size-1));
    MPI_Status *status = malloc(sizeof(MPI_Status) * (size-1));

    readImage(argv[1],imagemIn);

    createImage(imagemOut, imagemIn->row, imagemIn->col, imagemIn->max_gray);
    printf("dados da imagemOut: \nrow: %d\ncol: %d\n\n", imagemOut->row, imagemOut->col);

    int image_size = imagemIn->row * imagemIn->col;
    int worker_size = size - 1;
    int part_size = image_size / worker_size;
    int sub_idx = imagemOut->row / worker_size;
    puts("estou mandando o tamanho da matriz");
    MPI_Bcast(&imagemIn->row, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&imagemIn->col, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(imagemIn->matrix, image_size,
              MPI_INT, MASTER, MPI_COMM_WORLD);
    for (int i=1; i<size; i++) {
      MPI_Irecv(&imagemOut->matrix[(i-1)*sub_idx*imagemOut->col], part_size, MPI_INT, i,
                MATRIX, MPI_COMM_WORLD, &request[i-1]);
    }

    MPI_Waitall(worker_size, request, status);

    puts("recebi!!");
    free(request);
    free(status);

    // Processa os dados da imagem para a deteccao de borda
    puts("vou escrever a imagem");
    writeImage(argv[2],imagemOut,0);

    free(imagemIn);
    free(imagemOut);

  } else {
    int mat_size, cols, rows;
    MPI_Bcast(&rows, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    printf("recebi o tamanho da matriz, sou o filho %d, e a matriz tem tamanho %d x %d\n",
           rank, rows, cols);
    mat_size = cols * rows;
    int part_size = rows / (size-1);
    printf("sou o filho %d e o meu part size eh %d\n", rank, part_size);

    int *mat = (int *) malloc(sizeof(int) * mat_size);
    MPI_Bcast(mat, mat_size, MPI_INT, MASTER, MPI_COMM_WORLD);
    int *matout = (int *) malloc(sizeof(int) * mat_size);

    int start = (rank-1) * part_size;
    int tstart = (rank != 1) ? start - 1 : start;
    int end = (rank == size-1) ? rows : rank * part_size; // para valores quebrados
    int tend = (rank == size-1) ? rows : end + 1; // para valores quebrados
    printf("sou o filho %d e vou comecar a calcular com start=%d e end=%d\n",
           rank, start, end);
    edgeFilter(mat, matout, tstart, tend, 0, cols);
    printf("sou o filho %d e ja fiz a minha parte!\n", rank);

    MPI_Send(&matout[start*cols], part_size*cols, MPI_INT,
             MASTER, MATRIX, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
