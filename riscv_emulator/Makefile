CROSS=
CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -g

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c cpu.c util.c elfLoader.c


clean:
	rm -f main