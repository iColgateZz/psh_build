CFLAGS=-Wall -Wextra -O2 -Iinclude
CC=gcc
EXE=app

.PHONY: clean

all:
	$(CC) $(CFLAGS) -o $(EXE) main.c

clean:
	rm $(EXE)
