CC=g++
CFLAGS=-Wall -std=c++11
DEBUGFLAG=-DDEBUG
SHELL=/bin/sh

all: pugixml.o main.o tokenizer.o wordtrie.o parser.o
	$(CC) $(CFLAGS) -o aps02 pugixml.o main.o tokenizer.o wordtrie.o parser.o

debug: pugixml.o main.o debug/tokenizer.o debug/wordtrie.o debug/parser.o
	$(CC) $(CFLAGS) -o aps02 pugixml.o main.o tokenizer.o wordtrie.o parser.o

pugixml.o: pugixml/pugiconfig.hpp pugixml/pugixml.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c pugixml/pugixml.cpp

main.o: main.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c main.cpp

tokenizer.o: tokenizer.cpp tokenizer.hpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c tokenizer.cpp

debug/parser.o: parser.cpp tokenizer.hpp parser.hpp wordtrie/wordtrie.hpp
	$(CC) $(CFLAGS) $(DEBUGFLAG) -c parser.cpp

debug/tokenizer.o: tokenizer.cpp tokenizer.hpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) $(DEBUGFLAG) -c tokenizer.cpp

parser.o: parser.cpp tokenizer.hpp parser.hpp wordtrie/wordtrie.hpp
	$(CC) $(CFLAGS) -c parser.cpp

wordtrie.o: wordtrie/wordtrie.hpp wordtrie/wordtrie.cpp
	$(CC) $(CFLAGS) -c wordtrie/wordtrie.cpp

debug/wordtrie.o: wordtrie/wordtrie.hpp wordtrie/wordtrie.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAG) -c wordtrie/wordtrie.cpp

clean:
	rm -rf *.o aps02
