CC = gcc
MATH = -lm
CFLAGS = -Wall
OPENMP = -fopenmp
FLAGS = $(OPENMP) $(MATH) $(CFLAGS)

all: main.o parallel_board
	$(CC) -o board board.o main.o $(FLAGS)

serial: serial_board serial_main
	$(CC) -o board board.o main.o $(MATH) $(CFLAGS)

main.o: main.c board.h
	$(CC) -c main.c $(FLAGS)

serial_main: main.c board.h
	$(CC) $(CFLAGS) -c main.c

parallel_board: board.c board.h
	$(CC) -c board.c $(FLAGS)

serial_board: board.c board.h
	$(CC) -c board.c $(MATH) $(CFLAGS)

clean:
	rm *.o board
