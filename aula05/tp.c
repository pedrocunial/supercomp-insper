#include <stdio.h>
#include <omp.h>

#pragma omp threadprivate(counter)
int counter = 0;

int main(void)
{
#pragma omp parallel for num_threads(4)
    for (int i=0; i<16; i++) {
        printf("counter for thread %d: %d\n", omp_get_thread_num(), ++counter);
    }
}
