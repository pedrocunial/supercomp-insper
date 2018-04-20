CC=g++
CFLAGS=-Wall

all: pugixml.o main.o tokenizer.o
	$(CC) $(CFLAGS) -o aps02 pugixml.o main.o tokenizer.o

pugixml.o: pugixml/pugiconfig.hpp pugixml/pugixml.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c pugixml/pugixml.cpp

main.o: main.cpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c main.cpp

tokenizer.o: tokenizer.cpp tokenizer.hpp pugixml/pugixml.hpp
	$(CC) $(CFLAGS) -c tokenizer.cpp
