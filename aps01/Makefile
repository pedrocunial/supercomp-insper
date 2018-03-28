CC = gcc
CFLAGS = -Wall
OPENMP = -fopenmp

all: board.o main.o
	$(CC) $(CFLAGS) -o board board.o main.o

main.o: main.c board.h
	$(CC) $(CFLAGS) -c main.c

board.o: board.c board.h
	$(CC) $(CFLAGS) -c board.c

clean:
	rm *.o board
