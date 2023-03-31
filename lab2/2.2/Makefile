.PHONY: all clean

all: ask2 clean

CC = gcc
CFLAGS = -g -Wall -O2
SHELL= /bin/bash

ask2: ask2.o proc-common.o tree.o
	$(CC) $(CFLAGS) $^ -o $@

%.s: %.c
	$(CC) $(CFLAGS) -S -fverbose-asm $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.i: %.c
	gcc -Wall -E $< | indent -kr > $@

clean:
	rm ask2.o tree.o proc-common.o