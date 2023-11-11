.PHONY: microbench clean
CC = gcc
CFLAGS = -O0 -DDEFAULT

all: microbench

microbench: microbench.c
	$(CC) $(CFLAGS) -DDEFAULT  microbench.c -o microbench -pthread

clean:
	rm -rf microbench

