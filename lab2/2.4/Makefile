.PHONY: all clean

all: ask4

CC = gcc
CFLAGS = -g -Wall -O2
SHELL= /bin/bash

ask4: ask4.o proc-common.o tree.o
	$(CC) $(CFLAGS) $^ -o $@

%.s: %.c
	$(CC) $(CFLAGS) -S -fverbose-asm $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.i: %.c
	gcc -Wall -E $< | indent -kr > $@

clean:
	rm -f ask4.o proc-common.o tree.o
