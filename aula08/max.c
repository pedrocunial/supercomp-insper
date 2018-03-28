#include "mpi.h"
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define MASTER  0
#define N       5000
#define TAG_ARR 3
#define TAG_ID  2

int main(void)
{
    int size, rank;

    MPI_Request *request;
    MPI_Status  *status;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    request = (MPI_Request *) calloc(size * 2, sizeof(MPI_Request));
    status = (MPI_Status *) calloc(size * 2, sizeof(MPI_Status));

    if (size < 2) {
        puts("Escolha mais processos");
        return 1;
    }

    int patr_size = N / (size - 1);

    if (rank == MASTER) {
        // gen array
        int arr[N];
        srand(time(NULL));
        for (unsigned i=0; i<N; i++) {
            arr[i] = rand();
        }

        int *ans = (int *) calloc((size-1), sizeof(int));

        for (unsigned i=0; i<(size-1); i++) {
            int ptr_inc = i * patr_size;
            MPI_Isend(arr+ptr_inc, patr_size, MPI_INT, i+1, TAG_ARR,
                      MPI_COMM_WORLD, &request[i*2]);
            MPI_Irecv(&ans[i], 1, MPI_INT, i+1, TAG_ID, MPI_COMM_WORLD, &request[i*2+1]);
        }

        MPI_Waitall((size-1) * 2, request, status);

        int max = ans[0];
        int maxi = 0;
        for (unsigned i=1; i<size; i++) {
            if (ans[i] > max) {
                max = ans[i];
                maxi = i;
            }
        }

        printf("Max: %d, from part %d\n", max, maxi);

        free(ans);
    } else {
        int *arr = (int *) calloc(patr_size, sizeof(int));
        MPI_Irecv(arr, patr_size, MPI_INT, MASTER, TAG_ARR, MPI_COMM_WORLD, &request[0]);
        MPI_Wait(&request[0], &status[0]);

        int max = INT_MIN;
        for (unsigned i=0; i<patr_size; i++) {
            if (arr[i] > max) max = arr[i];
        }
        MPI_Isend(&max, 1, MPI_INT, MASTER, TAG_ID, MPI_COMM_WORLD, &request[1]);
        free(arr);
    }

    MPI_Finalize();
    return 0;
}
