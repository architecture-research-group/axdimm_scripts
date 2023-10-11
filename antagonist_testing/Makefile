.PHONY:map_test
CC = gcc
CFLAGS = -O0 -DDEFAULT

all: antagonist worker

sweeper: sweeper.c
	$(CC) -DDEFAULT  sweeper.c -o sweeper

antagonist: antagonist.c
	$(CC) -DDEFAULT  antagonist.c -o antagonist

worker: worker.c
	$(CC) -DDEFAULT  worker.c -o worker

clean:
	rm -rf antagonist worker

