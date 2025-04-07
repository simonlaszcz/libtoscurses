#
# Top level Makefile for curses widgets in cross-development env
#
#

CROSSDIR = /net/acae127/home/bammi/atari/cross-gcc
CROSSLIB = $(CROSSDIR)/lib
CROSSBIN = $(CROSSDIR)/bin
CROSSINC = $(CROSSDIR)/include

all: widget16.olb widget.olb

widget16.olb:
	make -f Makefile.16 clean
	make -f Makefile.16

widget.olb:
	make -f Makefile.32 clean
	make -f Makefile.32
wpage:
	make -f Makefile.16 wpage

wpage32:
	make -f Makefile.32 wpage32

clean:
	make -f Makefile.16 clean
	make -f Makefile.32 clean
	rm -f core report *.o 

install:
	cp widget16.olb widget.olb $(CROSSLIB)
	cp widget.h $(CROSSINC)
