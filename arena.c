#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

#define next_multiple(a, b) ((a) + (b) - (a) % (b))
#define MEM_ALIGNMENT sizeof(void*)
#define align(n) ((n) % MEM_ALIGNMENT == 0 ? (n) : next_multiple(n, MEM_ALIGNMENT))

// The arena region itself is allocated after the contents of the struct.
struct Arena {
	size_t size;
	void *last_block;
	void *next_block;
};

// Get the start address of the arena
static inline void *Arena_start(Arena *arena) {
	return arena + 1;
}

// Takes the size of the arena in bytes.
Arena *Arena_new(size_t size) {
	Arena *new_arena = malloc(sizeof(Arena) + size);

	*new_arena = (Arena) {
		.size = size,
		.last_block = NULL,
		.next_block = Arena_start(new_arena)
	};

	return new_arena;
}

void Arena_delete(Arena *arena) {
	free(arena);
}

static void print_diagnostic(Arena *arena, size_t size) {
	fprintf(stderr, "Diagnostic info:\n");
	fprintf(stderr, "Arena size: %zu bytes\n", arena->size);
	fprintf(stderr, "Amount currently allocated: %zu bytes\n", arena->next_block - Arena_start(arena));
	fprintf(stderr, "New block size: %zu bytes\n", size);
	fprintf(stderr, "New size upon success: %zu bytes\n", arena->next_block + align(size) - Arena_start(arena));
}

// Will return a null pointer if you've tried allocating too much memory.
void *Arena_alloc(Arena *arena, size_t size) {
	if (arena->next_block + align(size) > Arena_start(arena) + arena->size) {
		fprintf(stderr, "Allocation too large. You've attempted to allocate a block of memory past the end of the arena.\n");
		print_diagnostic(arena, size);
		return NULL;
	} else {
		void *new_block = arena->next_block;
		arena->last_block = new_block;
		arena->next_block += align(size);
		return new_block;
	}
}

/* Copy a block of memory into an arena.
 * Functionally equivalent to memcpy. */
void *Arena_copy(Arena *arena, const void *src, size_t size) {
	void *new_block = Arena_alloc(arena, size);
	if (new_block == NULL) {
		return NULL;
	} else {
		memcpy(new_block, src, size);
		return new_block;
	}
}

/* If the pointer is to the last allocation, it will be resized.
 * Otherwise, a new allocation will be created.
 * Be careful with this! A null pointer will be returned upon error.
 * Using this with memory outside of the arena is undefined behavior. */
void *Arena_realloc(Arena *arena, void *ptr, size_t size) {
	if (ptr == arena->last_block) {
		if (arena->last_block + align(size) > Arena_start(arena) + arena->size) {
			fprintf(stderr, "Allocation too large. You've attempted to allocate a block of memory past the end of the arena.\n");
			print_diagnostic(arena, size - (arena->next_block - arena->last_block));
			return NULL;
		} else {
			arena->next_block = arena->last_block + align(size);
			return ptr;
		}
	} else {
		return Arena_copy(arena, ptr, size);
	}
}
