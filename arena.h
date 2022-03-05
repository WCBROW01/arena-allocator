#ifndef ARENA_H
#define ARENA_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Arena Arena;

// Allocates a fixed-size arena. Accepts the size of the arena in bytes.
Arena *Arena_new(size_t size);

/* Allocates a dynamically-sized arena. Accepts the initial size of the arena in bytes.
 * If there is not enough space in the arena for an allocation, a new region will be created. */
Arena *Arena_new_dynamic(size_t size);

// Frees the entire arena from memory.
void Arena_delete(Arena *arena);

// Will return a null pointer if you've tried allocating too much memory.
void *Arena_alloc(Arena *arena, size_t size);

/* Copy a block of memory into an arena.
 * Functionally equivalent to memcpy. */
void *Arena_copy(Arena *arena, const void *src, size_t size);

/* If the pointer is to the last allocation, it will be resized.
 * Otherwise, a new allocation will be created.
 * Be careful with this! A null pointer will be returned upon error.
 * Using this with memory outside of the arena is undefined behavior. */
void *Arena_realloc(Arena *arena, void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
