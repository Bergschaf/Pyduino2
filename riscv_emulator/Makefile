CROSS=
CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -g

all: main


main: main.c cpu.o util.o elfLoader.o
	$(CC) $(CFLAGS)  main.c cpu.o util.o elfLoader.o -o main

elfLoader.o: elfLoader.c
	$(CC) $(CFLAGS) -c elfLoader.c

util.o: util.c
	$(CC) $(CFLAGS) -c util.c

cpu.o: cpu.c
	$(CC) $(CFLAGS) -c cpu.c

clean:
	rm -f main