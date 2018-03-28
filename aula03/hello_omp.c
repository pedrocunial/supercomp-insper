#include <omp.h>
#include <stdio.h>

#define NTHREADS 8

static long num_steps = 100000000;
double step;

int main(void)
{
  double start_time, end_time;
  double total = 0;
  long N = num_steps;

  step = 1.0 / (double) num_steps;
  num_steps /= NTHREADS;
  start_time = omp_get_wtime();

#pragma omp parallel num_threads(NTHREADS)
  {
    int thread_num = omp_get_thread_num();
    int start_i = (thread_num * num_steps) + 1;
    int end_i = num_steps * (thread_num + 1);
    double x, sum = 0;
    for (int i=start_i; i<=end_i; i++) {
      x = (i - .5) * step;
      sum += 4.0 / (1.0 + x * x);
    }
#pragma omp critical
    total += sum * step;
  }

  end_time = omp_get_wtime() - start_time;
  printf("simple parallel pi with %ld steps is %.12lf in %lf seconds\n",
         num_steps * NTHREADS, total, end_time);

  start_time = omp_get_wtime();
  double sum;
#pragma omp parallel for reduction(+: sum)
  for (int i=0; i<N; i++) {
    double x = (i -.5) * step;
    sum += 4.0 / (1.0 + x * x);
  }

  end_time = omp_get_wtime() - start_time;
  printf("reduction parallel pi with %ld steps is %.12lf in %lf seconds\n",
         N, total, end_time);

  start_time = omp_get_wtime();
  sum = 0;
  for (int i=0; i<N; i++) {
    double x = (i -.5) * step;
    sum += 4.0 / (1.0 + x * x);
  }

  end_time = omp_get_wtime() - start_time;
  printf("original pi with %ld steps is %.12lf in %lf seconds\n",
         N, total, end_time);

  return 0;
}
