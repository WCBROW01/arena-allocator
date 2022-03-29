/* This stack allocator implementation was created by Will Brown (WCBROW01).
 * Orginal source can be found at: https://github.com/WCBROW01/arena-allocator
 * Licensed under the MIT License (c) 2022 Will Brown */

#include "arena.c"
#include "stack.h"

// Allocates a fixed-size stack. Accepts the size of the arena in bytes.
Stack *Stack_new(size_t size) {
	return Arena_new(size);
}

/* Allocates a dynamically-sized stack. Accepts the initial size of the stack in bytes.
 * If there is not enough space in the stack for an allocation, a new region will be created. */
Stack *Stack_new_dynamic(size_t size) {
	return Stack_new_dynamic(size);
}

// Frees the entire stack from memory.
void Stack_delete(Stack *stack) {
	return Arena_delete(stack);
}

// Will return a null pointer if you've tried allocating too much memory.
void *Stack_alloc(Stack *stack, size_t size) {
	Arena_tmp_begin(stack);
	return Arena_alloc(stack, size + sizeof(void*));
}

/* Will free the last block of memory allocated in the arena if the pointer
 * passed in points to it. Otherwise, it does nothing.
 * Returns 1 if the block was freed, 0 if an invalid pointer was given. */
int Stack_free(Stack *stack, void *ptr) {
	if (stack->last_block == NULL || ptr != stack->last_block) {
		return 0;
	} else {
		Arena_tmp_rewind(stack);
		return 1;
	}
}

/* Copy a block of memory into an arena.
 * Functionally equivalent to memcpy. */
void *Stack_copy(Stack *stack, const void *src, size_t size) {
	Arena_tmp_begin(stack);
	return Arena_copy(stack, src, size);
}

/* If the pointer is to the last allocation, it will be resized.
 * Otherwise, a new allocation will be created.
 * Be careful with this! A null pointer will be returned upon error.
 * Using this with memory outside of the arena is undefined behavior. */
void *Stack_realloc(Stack *stack, void *ptr, size_t size) {
	if (stack->last_block != ptr) Arena_tmp_begin(stack);
	return Arena_realloc(stack, ptr, size);
}
