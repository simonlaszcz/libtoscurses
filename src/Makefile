#
# Top level Makefile for TOS-Gcc curses library
#

# GCC configuration info...

# Leave the next line undefined for native compilation setup.
CROSSDIR := /net/acae127/home/bammi/atari/cross-gcc

# Define MINTFLAG for MiNT versions of the libraries
#MINTFLAG := -mint

ifdef MINTFLAG
MINTOPT := -mint
m       := m
else
MINTOPT :=
m       :=
endif

ifdef CROSSDIR

# Set up to do cross-compiling...

CROSSLIB := $(CROSSDIR)/lib
CROSSBIN := $(CROSSDIR)/bin
CROSSINC := $(CROSSDIR)/include

AR    := $(CROSSBIN)/car
AS    := cgcc $(MINTOPT)
CC    := cgcc $(MINTOPT)
LIB   := $(CROSSLIB)
INC   := $(CROSSINC)
CP    := cp

else

# Set up to run native...

AR    := gcc-ar
AS    := gcc $(MINTOPT)
CC    := gcc $(MINTOPT)
LIB   := c:\gnu\lib
INC   := c:\gnu\include
CP    := cp

endif

# Additional options for compiling C code

#XFLAGS := -I../co
DEFINES := -DDGK -DGEMDOS

# Dynamic selection of options for each target library...

ifneq (,$(findstring debug,$(TARGET)))
OPTIM= -O -g -Wstrict-prototypes
FRAME=
DEBUG=
g=g
else
OPTIM= -O2 -fstrength-reduce
FRAME= -fomit-frame-pointer
DEBUG= -DNDEBUG
g=
endif

ifneq (,$(findstring long-int,$(TARGET)))
SHORT=
DSHORT=
ST=
else
SHORT= -mshort
DSHORT= -DSHORTLIB
ST=16
endif

ifneq (,$(findstring 68020,$(TARGET)))
CC := $(CC) -m68020
endif

ifneq (,$(findstring baserel,$(TARGET)))
CC := $(CC) -mbaserel
b=b
else
b=
endif

# common subset of options; no int size or omit-frame-pointer:
COPTS= $(DEBUG) $(OPTIM) $(DEFINES) $(XFLAGS) -I.

# Base options CC; includes int size but not omit-frame-pointer
CFFLAGS = $(SHORT) $(COPTS)

# normal CFLAGS including int size and omit-frame-pointer
CFLAGS= $(CFFLAGS) $(FRAME)

# cflags for stuff that needs to be compiled with 32 bit ints
CLFLAGS= $(COPTS) $(FRAME) $(DSHORT)

# flags to $(CC) when it runs the assembler only
ASFLAGS= -c

# flags to $(CC) when running preprocessor and assembler
PPFLAGS= $(SHORT) -P -c

# library targets
CURSES=$b$mcurses$g$(ST).olb
TERMCAP=$b$mtermcap$g$(ST).olb

ALL= $(CURSES) $(TERMCAP)
TESTS= ttcp$(ST).ttp twinkl$(ST).ttp worms$(ST).ttp

#all: lshort llong lgshort lglong
top: lshort llong

lshort:
	$(MAKE) clean
	$(MAKE) install

llong:
	$(MAKE) clean
	$(MAKE) install TARGET=long-int

lgshort:
	      $(MAKE) clean
	      $(MAKE) install TARGET=debug

lglong:
	      $(MAKE) clean
	      $(MAKE) install TARGET="long-int debug"

l020short:
	      $(MAKE) clean
	      $(MAKE) install020 TARGET=68020

l020long:
	      $(MAKE) clean
	      $(MAKE) install020 TARGET="68020 long-int"

lbshort:
	      $(MAKE) clean
	      $(MAKE) install TARGET="baserel"

lblong:
	      $(MAKE) clean
	      $(MAKE) install TARGET="long-int baserel"


# Get the rules to build everything...

include mincl
