#
# Makefile
# 

CC = gcc

# CAUTION: Always use '-pthread' when compiling POSIX threads-based
# applications, instead of linking with "-lpthread" directly.
CFLAGS = -Wall -O2 -pthread
LIBS = 

all: mandel-buffer mandel-procs


## Mandel

mandel-buffer: mandel-lib.o mandel-buffer.o
	$(CC) $(CFLAGS) -o mandel-buffer mandel-lib.o mandel-buffer.o $(LIBS)

mandel-procs: mandel-lib.o mandel-procs.o
	$(CC) $(CFLAGS) -o mandel-procs mandel-lib.o mandel-procs.o $(LIBS)

mandel-lib.o: mandel-lib.h mandel-lib.c
	$(CC) $(CFLAGS) -c -o mandel-lib.o mandel-lib.c $(LIBS)

mandel-buffer.o: mandel-buffer.c
	$(CC) $(CFLAGS) -c -o mandel-buffer.o mandel-buffer.c $(LIBS)

mandel-procs.o: mandel-procs.c
	$(CC) $(CFLAGS) -c -o mandel-procs.o mandel-procs.c $(LIBS)

clean:
	rm -f *.s *.o mandel-buffer mandel-procs
