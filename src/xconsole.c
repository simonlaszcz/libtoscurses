/* Replacement console read/write routines that allow for key
   remappings */
/* 
 * console input/output routines.
 * written by Eric R. Smith and placed in the public domain
 */

/*
 * BUGS: assumes all input is coming from the same place;
 * this bug shows up only when keys are remapped, though.
 */

#include <osbind.h>

#ifndef __MINT__
#include <support.h>
#include <stdlib.h>
#include <ioctl.h>
#include <tchars.h>
#include <signal.h>
#include <unixlib.h>
#include <keycodes.h>
#include <string.h>

#define STRDUP(x,y) if (x) free(x); x = strdup(y);

#define S_SHIFT 0x03
#define S_CNTRL	0x04
#define S_ALT	0x08

extern int _console_dev;	/* in main.c */
int __check_signals = 0;

/* arrays to hold keyboard strings */

static char *kb_unshft[N_KEYCODES], *kb_shft[N_KEYCODES], *kb_alt[N_KEYCODES];
static char *_str = NULL;

void console_set_key(keycode, regular, shifted, alted)
int keycode;
char *regular, *shifted, *alted;
{
	if (keycode < 1 || keycode >= N_KEYCODES)
		return;

	if (regular) {
		if (!*regular) regular = 0;
		STRDUP(kb_unshft[keycode],regular);
	}
	if (shifted) {
		if (!*shifted) shifted = 0;
/* shifted function keys are a bit screwy */
		if (keycode >= F_1 && keycode <= F_10) {
			STRDUP(kb_shft[keycode - F_1 + 0x54],shifted);
		}
		else {
			STRDUP(kb_shft[keycode],shifted);
		}
	}
	if (alted) {
		if (!*alted) alted = 0;
		STRDUP(kb_alt[keycode],alted);
	}
}

#define KBUFSIZ 80
#define NUMDEV	3
	/* 0 == prn:, 1 == aux:, 2 == con: */

typedef struct _buffer {
	short head, tail;
	long  buffer[KBUFSIZ];
} k_buf;

static k_buf in_buf[NUMDEV];

#define in_dev(dev) ((dev)>2 ? _console_dev : (dev))

#define IN_BUF(dev) (&in_buf[in_dev(dev)])


/*
 * what handle means:: 0-2: BIOS handle, 3: stdout, 4: stderr
 */
static short LOOKUP __PROTO((short handle));
static long raw_in __PROTO((int dev));
static long raw_instat __PROTO((int dev));
void flush_key_buff __PROTO((int dev));
#if 0
static void raw_out __PROTO((int dev, int c));
#endif

static short
#ifdef __STDC__
LOOKUP(short handle)
#else
LOOKUP(handle)
short handle;
#endif
{
	switch(handle) {
	case -3:
	case -2:
	case -1:
	case 0:
		return handle + 3;
	case 2:
		return 4;
	case 1:
		if (isatty(0)) return 3;
		if (isatty(2)) return 4;
		/* else fall through */
	default:
		return _console_dev;
	}
}

/*
 * raw i/o routines
 */

static long
raw_in(dev)
int dev;
{
  if (dev < 3)
    return Bconin (dev);
  else if (dev == 3)
    return Crawcin ();
  else
    return Cauxin ();
}

#if 0
static void
raw_out(dev, c)
int dev, c;
{
	if (dev < 3)
		Bconout(dev, c);
	else if (dev == 3)
		Crawio(c);
	else
		Cauxout(c);
}
#endif

static long
raw_instat(dev)
int dev;
{
    return Bconstat(in_dev(dev));
}

/*
 * somewhat less raw i/o routines. The main difference is that these ones
 * will check for pending input before doing output, to see if a signal
 * needs to be raised. This is only done if __check_signals is non-zero;
 * signal() should set __check_signals when the user attempts to catch
 * SIGINT or SIGQUIT. We don't do this checking all the time because
 * the user may be typing ahead input for another program (e.g. if this
 * is a little utility of some sort) and we shouldn't steal keystrokes
 * unless necessary.
 */

unsigned int console_read_byte(handle)
int handle;
{
	k_buf *p;
	short i, j, dev;
	unsigned scan, key, shft;
	long r;

	dev = LOOKUP(handle);

	if (_str) {
		if (*_str)
			return *(unsigned char *)_str++;
		else
			_str = 0;
	}

	p = IN_BUF(dev);
	if ( p->head != (i = p->tail)) {
		j = p->tail + 1;
		if (j >= KBUFSIZ)
			j = 0;
		p->tail = j;
		r = p->buffer[i];
	}
	else
		r = raw_in(dev);

	scan = (r & 0x00ff0000) >> 16;
	key = r & 0x00ff;
	if (scan > 0 && scan < N_KEYCODES) {
		shft = Kbshift(-1);
		if (shft & S_ALT)
			_str = kb_alt[scan];
		else if (shft & S_SHIFT)
			_str = kb_shft[scan];
		else if (!(shft & S_CNTRL))
			_str = kb_unshft[scan];
		if (_str) return *(unsigned char *)_str++;
	}
	return key;
}

int console_input_status(handle)
int handle;
{
	short dev;
	k_buf *p;

	if (_str && *_str) return 1;
	dev = LOOKUP(handle);
	p = IN_BUF(dev);
	return (p->head != p->tail) || raw_instat(dev);
}

void
console_write_byte(handle, n)
int handle;
int n;
{
	char ch;

	if (__check_signals)
		flush_key_buff(handle);
	ch = n;
	(void)Fwrite(handle, 1L, &ch);
}

void flush_key_buff(fd)  
int fd;
{ 
    long c;
    short i, j, waiting = 0;
    unsigned char ch;
    k_buf *p;
    short dev;
    short handle;
    
    handle = fd;
    
    dev = LOOKUP(handle);
    p = IN_BUF(dev);
    
    while (raw_instat(dev) || waiting) 
    { 
	c = raw_in(dev);
	if (!(__ttymode & RAW)) 
	{
	    ch = c & 0xff;
	    if (ch == ('S'-'@')) { waiting = 1; }
	    else if (ch == ('Q'-'@')) { waiting = 0; }
	    else if (ch == __tchars[TC_INTRC]) 
	    { 
		p->head = p->tail;
		raise(SIGINT);
	    }
	    else if (ch == __tchars[TC_QUITC]) 
	    { 
		p->head = p->tail;
		raise(SIGQUIT);
	    }
	    else if (!waiting) 
	    { 
		i = p->head;
		j = i + 1;
		if (j >= KBUFSIZ) j = 0;
		if (j != p->tail) 
		{ 
		    p->buffer[i] = c;
		    p->head = j;
		}
	    }
	}
    }
}

#else  /* __MINT__ */

/* a rough approximation for console_set_key using MiNT's
 * XKEY ioctl
 */

#include <ioctl.h>
#include <keycodes.h>
#include <string.h>

static struct DEFS {
	int kcode, reg, shift;
} codes[] = {
{ CURS_UP, 20, 28 },
{ CURS_DN, 21, 29 },
{ CURS_RT, 22, 30 },
{ CURS_LF, 23, 31 },
{ K_HELP, 24, 0 },
{ K_UNDO, 25, 0 },
{ K_INS, 26, 0 },
{ K_HOME, 27, 0 },
{ 0, 0, 0 }
};

void
console_set_key(keycode, regular, shifted, alted)
int keycode;
char *regular, *shifted, *alted;
{
	struct Xkey {
		short xk_num;
		char xk_def[8];
	} xk;
	int i;

	xk.xk_def[7] = 0;
	if (keycode >= F_1 && keycode <= F_10) {
		if (regular) {
			xk.xk_num = keycode - F_1;
			strncpy(xk.xk_def, regular, 7);
			ioctl(0, TIOCSXKEY, &xk);
		}
		if (shifted) {
			xk.xk_num = keycode - F_1 + 10;
			strncpy(xk.xk_def, regular, 7);
			ioctl(0, TIOCSXKEY, &xk);
		}
		return;
	}
	for (i = 0; codes[i].kcode; i++) {
		if(keycode == codes[i].kcode) {
			if (regular) {
				xk.xk_num = codes[i].reg;
				strncpy(xk.xk_def, regular, 7);
				ioctl(0, TIOCSXKEY, &xk);
			}
			if (shifted && codes[i].shift) {
				xk.xk_num = codes[i].shift;
				strncpy(xk.xk_def, shifted, 7);
				ioctl(0, TIOCSXKEY, &xk);
			}
			return;
		}
	}
}

#endif /* __MINT__ */
