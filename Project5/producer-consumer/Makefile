
# makefile for producer-consumer problem
#

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

example: main.o buffer.o
	$(CC) $(CFLAGS) -o example main.o buffer.o $(PTHREADS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(PTHREADS)

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAG) -c buffer.c

clean: 
	rm -rf *.o
	rm -rf example
