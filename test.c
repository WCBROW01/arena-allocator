#include <stdio.h>

#include "arena.h"

int main(void) {
	printf("Allocating an arena.\n");
	Arena *arena = Arena_new(1024);
	printf("Arena location: %p\n", arena);

	printf("\nAllocating a new variable foo in the arena.\n");
	int *foo = Arena_alloc(arena, sizeof(int));
	printf("Location of foo: %p\n", foo);
	*foo = 69;
	printf("Contents of foo: %d\n", *foo);

	printf("\nResizing foo to make an array.\n");
	Arena_realloc(arena, foo, 4 * sizeof(int));
	printf("Location of foo: %p\n", foo);
	for (int i = 0; i < 4; i++) foo[i] = i;
	printf("Elements of foo:\n");
	for (int i = 0; i < 4; i++) printf("%d ", foo[i]);
	printf("\n");

	printf("\nMaking a copy of foo called bar.\n");
	int *bar = Arena_copy(arena, foo, 4 * sizeof(int));
	printf("Location of bar: %p\n", bar);
	printf("Elements of bar:\n");
	for (int i = 0; i < 4; i++) printf("%d ", bar[i]);
	printf("\n");

	printf("\nMaking a new variable baz\n");
	long *baz = Arena_alloc(arena, sizeof(long));
	printf("Location of baz: %p\n", baz);
	*baz = 0xFFFFFFFF;
	printf("Contents of baz: %ld\n", *baz);

	printf("\nAttempting to overflow the arena.\n");
	Arena_alloc(arena, 1024);
	Arena_delete(arena);

	printf("\nAllocating a dynamic arena.\n");
	Arena *dynamic_arena = Arena_new_dynamic(1024);
	printf("Arena location: %p\n", dynamic_arena);

	printf("\nCreating a new variable qux in the dynamic arena.\n");
	int *qux = Arena_alloc(dynamic_arena, sizeof(int));
	printf("Location of qux: %p\n", qux);

	printf("\nCreating a huge allocation in the dynamic arena.\n");
	char *huge = Arena_alloc(dynamic_arena, 2048);
	printf("Location of huge: %p\n", huge);

	printf("\nCreating a new, smaller allocation in the dynamic arena.\n");
	char *small = Arena_alloc(dynamic_arena, 256);
	printf("Location of small: %p\n", small);

	Arena_delete(dynamic_arena);
	return 0;
}
