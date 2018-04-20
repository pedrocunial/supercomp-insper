CC=gcc-7
CFLAGS=-Xpreprocessor -fopenmp -lomp --std=c99 -pedantic -Wall

all: main.o lin_regression.o
	${CC} ${CFLAGS} lin_regression.o main.o -o q2

main.o: main.c
	${CC} ${CFLAGS} -c main.c

lin_regression.o: lin_regression.c
	${CC} ${CFLAGS} -c lin_regression.c
