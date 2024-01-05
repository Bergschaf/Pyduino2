CROSS=
CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -g

all: main

main: main.c decoder.c
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm -f main