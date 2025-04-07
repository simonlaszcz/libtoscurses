# simple makefile for bandit
CC = mgcc

ALL = bandit bandit32

all: $(ALL)

bandit:	bandit.c
	$(CC) -o bandit $(CFLAGS) bandit.c -lcurses -mshort -s

bandit32: bandit.c
	$(CC) -o bandit32 $(CFLAGS) bandit.c -lcurses32 -s

clean:
	rm -f $(ALL) *.o report
