CROSS=
CC=gcc
CFLAGS=-Wall -Wextra  -pedantic -g -rdynamic

all: main debug


main: main.c cpu.o util.o elfLoader.o decoder.o instructions.o kernel.o
	$(CC) $(CFLAGS)  main.c cpu.o util.o elfLoader.o instructions.o decoder.o kernel.o  -o main

debug: debug.c cpu.o util.o elfLoader.o decoder.o instructions.o kernel.o
	$(CC) $(CFLAGS) debug.c cpu.o util.o elfLoader.o instructions.o decoder.o kernel.o  -o debug

elfLoader.o: elfLoader.c
	$(CC) $(CFLAGS) -c elfLoader.c

util.o: util.c
	$(CC) $(CFLAGS) -c util.c

cpu.o: cpu.c config.h
	$(CC) $(CFLAGS) -c cpu.c

instructions.o: instructions.c
	$(CC) $(CFLAGS) -c instructions.c

decoder.o: decoder.c instructions.c
	$(CC) $(CFLAGS) -c decoder.c instructions.o -o decoder.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c



clean:
	rm -f main