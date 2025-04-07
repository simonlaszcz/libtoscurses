#
# Top level Makefile for TOS-Gcc curses library in unix hosted cross dev env.
#

CROSSDIR = /net/acae127/home/bammi/atari/cross-gcc
CROSSLIB = $(CROSSDIR)/lib
CROSSBIN = $(CROSSDIR)/bin
CROSSINC = $(CROSSDIR)/include

AR	= $(CROSSBIN)/car
CC	= cgcc -mint
CPP	= /lib/cpp
AS 	= $(CC)
LIB     = $(CROSSLIB)
INC	= $(CROSSINC)
MAKE	= /util/gnu/bin/make

MAKEFLAGS = CC='$(CC)' CPP='$(CPP)' AS='$(AS)' AR='$(AR)' \
 XFLAGS='$(XFLAGS)' LIB='$(LIB)' INC='$(INC)'

#all: lshort llong lgshort lglong
all: lshort llong

lshort:
	$(MAKE) -f Mmakefile.16 clean
	$(MAKE) -f Mmakefile.16 $(MAKEFLAGS) all 
	$(MAKE) -f Mmakefile.16 $(MAKEFLAGS) install

llong:
	$(MAKE) -f Mmakefile.32 clean
	$(MAKE) -f Mmakefile.32 $(MAKEFLAGS) all
	$(MAKE) -f Mmakefile.32 $(MAKEFLAGS) install

lgshort:
	$(MAKE) -f gMmakefile.16 clean
	$(MAKE) -f gMmakefile.16 $(MAKEFLAGS) all
	$(MAKE) -f gMmakefile.16 $(MAKEFLAGS) install

lglong:
	$(MAKE) -f gMmakefile.32 clean
	$(MAKE) -f gMmakefile.32 $(MAKEFLAGS) all
	$(MAKE) -f gMmakefile.32 $(MAKEFLAGS) install

clean:
	$(MAKE) -f Mmakefile.16 clean
	$(MAKE) -f Mmakefile.32 clean

realclean:
	$(MAKE) -f Mmakefile.16 realclean
	$(MAKE) -f Mmakefile.32 realclean
	rm -f core report
