CC=mpic++
CFLAGS=-Wall -std=c++11
MPIFLAGS=-I/usr/local/include/ -L/usr/local/lib -lboost_mpi -lboost_serialization
DEBUGFLAG=-DDEBUG
SHELL=/bin/sh

all: pugixml.o main.o default/tokenizer.o default/wordtrie.o default/parser.o
	$(CC) $(CFLAGS) $(MPIFLAGS) -o aps02 pugixml.o main.o tokenizer.o wordtrie.o parser.o

debug: pugixml.o debug/main.o debug/tokenizer.o debug/wordtrie.o debug/parser.o
	$(CC) $(CFLAGS) $(MPIFLAGS) $(DEBUGFLAG) -o aps02 pugixml.o main.o tokenizer.o wordtrie.o parser.o

pugixml.o: pugixml/pugiconfig.hpp pugixml/pugixml.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c pugixml/pugixml.cpp

main.o: main.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) $(MPIFLAGS) -c main.cpp

debug/main.o: main.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) $(MPIFLAGS) $(DEBUGFLAG) -c main.cpp

default/tokenizer.o: tokenizer.cpp tokenizer.hpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c tokenizer.cpp

debug/parser.o: parser.cpp tokenizer.hpp parser.hpp wordtrie/wordtrie.hpp
	$(CC) $(CFLAGS) $(DEBUGFLAG) $(MPIFLAGS) -c parser.cpp

debug/tokenizer.o: tokenizer.cpp tokenizer.hpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) $(DEBUGFLAG) -c tokenizer.cpp

default/parser.o: parser.cpp tokenizer.hpp parser.hpp wordtrie/wordtrie.hpp
	$(CC) $(CFLAGS) $(MPIFLAGS) -c parser.cpp

default/wordtrie.o: wordtrie/wordtrie.hpp wordtrie/wordtrie.cpp
	$(CC) $(CFLAGS) -c wordtrie/wordtrie.cpp

debug/wordtrie.o: wordtrie/wordtrie.hpp wordtrie/wordtrie.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAG) -c wordtrie/wordtrie.cpp

clean:
	rm -rf *.o aps02
