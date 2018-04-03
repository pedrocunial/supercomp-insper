CC = gcc
MATH = -lm
CFLAGS = -Wall
OPENMP = -fopenmp

all: main.o parallel_board
	$(CC) $(CFLAGS) -o board board.o main.o $(OPENMP) $(MATH)

main.o: main.c board.h
	$(CC) $(CFLAGS) -c main.c $(OPENMP)

parallel_board: board.c board.h
	$(CC) $(CFLAGS) -c board.c $(OPENMP)

rm_img:
	rm *.gif *.pgm

clean:
	rm *.o board *.pgm *.gif
