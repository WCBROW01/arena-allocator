CC=cc
CFLAGS=-Wall -Wextra -DENABLE_DIAG -O2

test: test.c arena.c
	$(CC) $(CFLAGS) -o test test.c arena.c
