#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void car(void)
{
    printf(" car");
}

void race(void)
{
    printf(" race");
}

void ex0(void)
{
    printf("I think race cars are cool\n");
    exit(0);
}

void ex1(void)
{
    printf("I think car races are cool\n");
    exit(0);
}

void topper_way(void)
{
#pragma omp parallel num_threads(2)
#pragma omp master
    {
#pragma omp task
        ex0();
#pragma omp task
        ex1();
    }
}

int pretty_way(void)
{
    printf("I think");
#pragma omp parallel num_threads(2)
#pragma omp master
    {
#pragma omp task
        race();
#pragma omp task
        car();
    }
    printf("s are cool\n");
    return 0;
}

int main(void)
{
    pretty_way();
}
