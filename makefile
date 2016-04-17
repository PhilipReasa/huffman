CC=g++
CFLAGS=-std=c++11 -Wall -pedantic

all: compile

test.o: test.cpp
	$(CC) $(CFLAGS) -c test.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

test: test.o
	$(CC) $(CFLAGS) test.o -o test

compile: main.o
	$(CC) $(CFLAGS) main.o -o run

clean:
	rm *.o 
	rm test