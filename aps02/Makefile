CC=g++
CFLAGS=-Wall -std=c++11
SHELL=/bin/sh

all: pugixml.o main.o tokenizer.o wordtrie.o parser.o
	$(CC) $(CFLAGS) -o aps02 pugixml.o main.o tokenizer.o wordtrie.o parser.o

pugixml.o: pugixml/pugiconfig.hpp pugixml/pugixml.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c pugixml/pugixml.cpp

main.o: main.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c main.cpp

tokenizer.o: tokenizer.cpp tokenizer.hpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c tokenizer.cpp

debug/parser.o: parser.cpp tokenizer.hpp parser.hpp wordtrie/wordtrie.hpp
	$(CC) $(CFLAGS) -dDEBUG -c parser.cpp

parser.o: parser.cpp tokenizer.hpp parser.hpp wordtrie/wordtrie.hpp
	$(CC) $(CFLAGS) -c parser.cpp

wordtrie.o: wordtrie/wordtrie.hpp wordtrie/wordtrie.cpp
	$(CC) $(CFLAGS) -c wordtrie/wordtrie.cpp

clean:
	rm -rf *.o aps02
