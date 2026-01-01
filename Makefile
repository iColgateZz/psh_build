CFLAGS=-Wall -Wextra -O2 -Iinclude -Iarena_allocator -Ipsh_core
CC=gcc
EXE=app

.PHONY: clean

all:
	$(CC) $(CFLAGS) -o $(EXE) main.c

clean:
	rm $(EXE)
