CROSS=
CC=gcc
CFLAGS=-Wall -Wextra  -pedantic -g -rdynamic

all: main debug


main: main.c cpu.o util.o elfLoader.o decoder.o instructions.o kernel.o config.h filesystem.o
	$(CC) $(CFLAGS)  main.c cpu.o util.o elfLoader.o instructions.o decoder.o filesystem.o kernel.o   -o main

debug: debug.c cpu.o util.o elfLoader.o decoder.o instructions.o kernel.o config.h filesystem.o
	$(CC) $(CFLAGS) debug.c cpu.o util.o elfLoader.o instructions.o decoder.o kernel.o filesystem.o  -o debug

debug_elfLoader: debug_elfLoader.c cpu.o util.o elfLoader.o decoder.o instructions.o kernel.o config.h
	$(CC) $(CFLAGS) debug_elfLoader.c cpu.o util.o elfLoader.o instructions.o decoder.o kernel.o  -o debug_elfLoader

elfLoader.o: elfLoader.c
	$(CC) $(CFLAGS) -c elfLoader.c

util.o: util.c
	$(CC) $(CFLAGS) -c util.c

cpu.o: cpu.c config.h
	$(CC) $(CFLAGS) -c cpu.c

instructions.o: instructions.c
	$(CC) $(CFLAGS) -c instructions.c -o instructions.o

decoder.o: decoder.c instructions.c
	$(CC) $(CFLAGS) -c decoder.c instructions.o -o decoder.o

kernel.o: kernel.c filesystem.o
	$(CC) $(CFLAGS)  -c kernel.c -o kernel.o

filesystem.o: filesystem.c filesystem.h
	$(CC) $(CFLAGS) -c filesystem.c -o filesystem.o



clean:
	rm -f *.o
	rm -f main
	rm -f debug
	rm -f debug_elfLoader