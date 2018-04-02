CC = gcc
MATH = -lm
CFLAGS = -Wall
OPENMP = -fopenmp

all: main.o parallel_board
	$(CC) $(CFLAGS) -o board board.o main.o $(OPENMP) $(MATH)

# serial: serial_board serial_main
#		$(CC) -o board board.o main.o $(CFLAGS)

main.o: main.c board.h
	$(CC) $(CFLAGS) -c main.c $(OPENMP)

# serial_main: main.c board.h
#		$(CC) $(CFLAGS) -c main.c

parallel_board: board.c board.h
	$(CC) $(CFLAGS) -c board.c $(OPENMP)

# serial_board: board.c board.h
#		$(CC) -c board.c $(MATH) $(CFLAGS)

clean:
	rm *.o board
