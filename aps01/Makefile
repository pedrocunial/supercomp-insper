CC = gcc
MATH = -lm
CFLAGS = -Wall
OPENMP = -fopenmp
FLAGS = $(OPENMP) $(MATH) $(CFLAGS)

all: board.o main.o
	$(CC) -o board board.o main.o $(FLAGS)

main.o: main.c board.h
	$(CC) $(CFLAGS) -c main.c

board.o: board.c board.h
	$(CC) -c board.c $(FLAGS)

clean:
	rm *.o board
