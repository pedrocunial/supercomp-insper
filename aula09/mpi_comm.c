// Author: Wesley Bland
// Copyright 2015 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// Example using MPI_Comm_split to divide a communicator into subcommunicators
//

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define MASTER 0
#define N   1000

float calc_min(float *arr, int start, int end) {
  min = arr[start];
  for (i=start+1; i<end; i++) {
    if (arr[i] < min) min = arr[i];
  }
  return min;
}

float calc_max(float *arr, int start, int end) {
  max = arr[start];
  for (i=start+1; i<end; i++) {
    if (arr[i] > max) max = arr[i];
  }
  return max;
}

// Creates an array of random numbers. Each number has a value from 0 - 1
float *create_rand_nums(int num_elements) {
  float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
  assert(rand_nums != NULL);
  int i;
  for (i = 0; i < num_elements; i++) {
    rand_nums[i] = (rand() / (float)RAND_MAX);
  }
  return rand_nums;
}

int main(int argc, char **argv) {
  MPI_Init(NULL, NULL);

  // Get the rank and size in the original communicator
  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  float *arr;
  if (world_rank == MASTER) {
    arr = create_rand_nums(N);
    MPI_Bcast(arr, N, MPI_FLOAT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Recv(arr, N, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  int color = (world_rank % 2 == 0) ? 0 : 1; // Determine color based on row

  // Split the communicator based on the color and use the original rank for ordering
  MPI_Comm row_comm;
  MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &row_comm);

  int row_rank, row_size;
  MPI_Comm_rank(row_comm, &row_rank);
  MPI_Comm_size(row_comm, &row_size);

  printf("WORLD RANK/SIZE: %2d/%d --- ROW RANK/SIZE: %d/%d\n",
    world_rank, world_size, row_rank, row_size);

  float min, max;
  if (color == 0) {
    float min_local = calc_min(arr, row_rank * N/row_size, (1+row_rank)*N/row_size);
    MPI_Reduce(&min, &arr, N, MPI_FLOAT, MPI_MIN, MASTER, &row_comm);
    if (row_rank == 0) {
      MPI_Send(&min, 1, MPI_FLOAT, MASTER, )
    }
  } else {
    float max_local = calc_max(arr, row_rank * N/row_size, (1+row_rank)*N/row_size);
    MPI_Reduce(&max, &arr, N, MPI_FLOAT, MPI_MAX, MASTER, &row_comm);
  }

  MPI_Comm_free(&row_comm);

  MPI_Finalize();
}
