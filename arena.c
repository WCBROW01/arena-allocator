#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
	Arena *next_region;
	bool dynamic;
};

// Get the start address of the arena
static inline void *Arena_start(Arena *arena) {
	return arena + 1;
}

// Allocates a fixed-size arena. Accepts the size of the arena in bytes.
Arena *Arena_new(size_t size) {
	Arena *new_arena = malloc(sizeof(Arena) + size);

	*new_arena = (Arena) {
		.size = size,
		.last_block = NULL,
		.next_block = Arena_start(new_arena),
		.next_region = NULL,
		.dynamic = false
	};

	return new_arena;
}

/* Allocates a dynamically-sized arena. Accepts the initial size of the arena in bytes.
 * If there is not enough space in the arena for an allocation, a new region will be created. */
Arena *Arena_new_dynamic(size_t size) {
	Arena *new_arena = malloc(sizeof(Arena) + size);

	*new_arena = (Arena) {
		.size = size,
		.last_block = NULL,
		.next_block = Arena_start(new_arena),
		.next_region = NULL,
		.dynamic = true
	};

	return new_arena;
}

// Frees the entire arena from memory.
void Arena_delete(Arena *arena) {
	if (arena->next_region != NULL) Arena_delete(arena->next_region);
	free(arena);
}

static void print_diagnostic(Arena *arena, size_t size) {
	fprintf(stderr, "Diagnostic info:\n");
	fprintf(stderr, "Arena size: %zu bytes\n", arena->size);
	fprintf(stderr, "Amount currently allocated: %zu bytes\n", arena->next_block - Arena_start(arena));
	fprintf(stderr, "New block size: %zu bytes\n", size);
	fprintf(stderr, "New size upon success: %zu bytes\n", arena->next_block + align(size) - Arena_start(arena));
}

static inline void *Arena_init_block(Arena *arena, size_t size) {
	void *new_block = arena->next_block;
	arena->last_block = new_block;
	arena->next_block += align(size);
	return new_block;
}

// Will return a null pointer if you've tried allocating too much memory.
void *Arena_alloc(Arena *arena, size_t size) {
	if (arena->next_block + align(size) > Arena_start(arena) + arena->size) {
		if (arena->dynamic) {
			if (arena->next_region == NULL) {
				// If the size is too large for a region, make a special region for only that block.
				size_t region_size = size > arena->size ? size : arena->size;
				arena->next_region = Arena_new_dynamic(region_size);
				return Arena_init_block(arena->next_region, size);
			} else {
				return Arena_alloc(arena->next_region, size);
			}
		} else {
			fprintf(stderr, "Allocation too large. You've attempted to allocate a block of memory past the end of the arena.\n");
			print_diagnostic(arena, size);
			return NULL;
		}
	} else {
		return Arena_init_block(arena, size);
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
			if (arena->dynamic) {
				return Arena_copy(arena, ptr, size);
			} else {
				fprintf(stderr, "Allocation too large. You've attempted to allocate a block of memory past the end of the arena.\n");
				print_diagnostic(arena, size - (arena->next_block - arena->last_block));
				return NULL;
			}
		} else {
			arena->next_block = arena->last_block + align(size);
			return ptr;
		}
	} else {
		return Arena_copy(arena, ptr, size);
	}
}
