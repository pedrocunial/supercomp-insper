/*
Este programa computará numericamente a integral de

                   4/(1 + x*x)

de 0 a 1. O valor desta integral é pi;
que é ótimo, uma vez que nos dá uma maneira fácil de verificar a resposta.

Esta versão do programa usa um algoritmo dividir e conquista e recursão.

History: Written by Tim Mattson, 10/2013
Atualizado por: Luciano Soares
*/

#include <omp.h>
#include <stdio.h>
static long num_steps = 1024*1024*1024;
#define MIN_BLK  1024*1024*256

double pi_comp(int Nstart,int Nfinish,double step) {
   int i,iblk;
   double x, sum = 0.0,sum1, sum2;
   if (Nfinish-Nstart < MIN_BLK){
      for (i=Nstart;i< Nfinish; i++){
         x = (i+0.5)*step;
         sum = sum + 4.0/(1.0+x*x);
      }
   }
   else{
      iblk = Nfinish-Nstart;
      sum1 = pi_comp(Nstart,         Nfinish-iblk/2,step);
      sum2 = pi_comp(Nfinish-iblk/2, Nfinish,       step);
      sum = sum1 + sum2;
   }
   return sum;
}

double pi_omp(int start, int end, double step)
{
    int i, iblk = end - start;
    double x, sum = 0, sum1, sum2;
    if (iblk < MIN_BLK) {
#pragma omp parallel for reduction(+: sum) default(none) firstprivate(end, start, step) private(i, x)
        for (i=start; i < end; i++) {
            x = (i + .5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
    } else {
#pragma omp task shared(sum1)
        sum1 = pi_omp(start, end  - iblk / 2, step);
#pragma omp task shared(sum2)
        sum2 = pi_omp(end - iblk / 2, end, step);
#pragma omp taskwait
        sum = sum1 + sum2;
    }
    return sum;
}

int main(void) {
   double step, pi, sum;
   double init_time, final_time;
   step = 1.0/(double) num_steps;

   init_time = omp_get_wtime();
#pragma omp parallel
#pragma omp master
   sum = pi_omp(0,num_steps,step);
   pi = step * sum;
   final_time = omp_get_wtime() - init_time;
   printf("for %ld steps pi = %f in %f secs\n",num_steps,pi,final_time);

   init_time = omp_get_wtime();
   sum = pi_comp(0,num_steps,step);
   pi = step * sum;
   final_time = omp_get_wtime() - init_time;
   printf("for %ld steps pi = %f in %f secs\n",num_steps,pi,final_time);

   return 0;
}
