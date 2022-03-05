CC=cc
CFLAGS=-Wall -Wextra -O2

test: test.c arena.c
	$(CC) $(CFLAGS) -o test test.c arena.c
