CROSS=
CC=gcc
CFLAGS=-Wall -Wextra  -pedantic -g -rdynamic

all: main


main: main.c cpu.o util.o elfLoader.o decoder.o instructions.o
	$(CC) $(CFLAGS)  main.c cpu.o util.o elfLoader.o instructions.o decoder.o  -o main

elfLoader.o: elfLoader.c
	$(CC) $(CFLAGS) -c elfLoader.c

util.o: util.c
	$(CC) $(CFLAGS) -c util.c

cpu.o: cpu.c
	$(CC) $(CFLAGS) -c cpu.c

instructions.o: instructions.c
	$(CC) $(CFLAGS) -c instructions.c

decoder.o: decoder.c instructions.c
	$(CC) $(CFLAGS) -c decoder.c instructions.o -o decoder.o


clean:
	rm -f main