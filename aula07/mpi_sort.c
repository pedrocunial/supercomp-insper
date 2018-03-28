#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int cmpfunc (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

void merge(int a[100], const int b[50]) {
    int c[100];
    int bi = 0, ai = 0;
    for (int i=0; bi<50 || ai<50; i++) {
        if (bi < 50 && ai < 50) {
            if (a[ai] > b[bi]) {
                c[i] = b[bi];
                bi++;
            } else {
                c[i] = a[ai];
                ai++;
            }
        } else if (ai < 50) {
            c[i] = a[ai];
            ai++;
        } else {
            c[i] = b[bi];
            bi++;
        }
    }
    for (int i=0; i<100; i++) {
        a[i] = c[i];
    }
}

int main(int argc, char ** argv) {

    int rank, a[100], b[50];
    MPI_Init(&argc, &argv);
    double start = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        for(int f=0;f<100;f++) a[f]=rand()%1000;
        MPI_Send(&a[50], 50, MPI_INT, 1, 0, MPI_COMM_WORLD);
        qsort(a, 50, sizeof(int), cmpfunc);
        MPI_Recv(b, 50, MPI_INT, 1, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        /* Atividade: termine de ordenar os dados */
        printf("before merge (a):\n");
        for(int f= 0;f< 50;f++) printf("%d\n",a[f]);
        printf("\nbefore merge (a):\n");
        for(int f= 0;f< 50;f++) printf("%d\n",b[f]);
        merge(a, b);
        printf("\nmerged:\n");
        for(int f=0; f<100; f++) printf("%d\n", a[f]);
        double end = MPI_Wtime();
        printf("tempo decorrido: %f\n",end - start);

    } else if (rank == 1) {
        MPI_Recv(b, 50, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        qsort(b, 50, sizeof(int), cmpfunc);
        MPI_Send(b, 50, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;

}
