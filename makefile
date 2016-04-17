CC=g++
CFLAGS=-std=c++11 -Wall -pedantic

all: compile

test.o: test.cpp
	$(CC) $(CFLAGS) -c test.cpp

compile: test.o
	$(CC) $(CFLAGS) test.o -o test

clean:
	rm *.o 
	rm test