# arena-allocator

A simple arena allocator written in C, contained within a single source and header pair.
Currently, the size of each arena is fixed, but dynamic arenas will be added in the future, where the provided size will become the maximum possible block size.

A makefile is provided for building a test program, but to use it, you just need to include the source and header files in your own program.
