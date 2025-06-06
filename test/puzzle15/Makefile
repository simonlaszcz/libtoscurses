# simple makefile for puzzle
CC = mgcc

ALL = puzzle puzzle32

all: $(ALL)

puzzle:	puzzle15.c
	$(CC) -o puzzle $(CFLAGS) puzzle15.c -lcurses -mshort -s

puzzle32: puzzle15.c
	$(CC) -o puzzle32 $(CFLAGS) puzzle15.c -lcurses32 -s

clean:
	rm -f $(ALL) *.o report
