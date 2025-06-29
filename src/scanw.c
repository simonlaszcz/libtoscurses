/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "internal.h"
#include <string.h>
#include <stdarg.h>

/*
 * This routine implements a scanf on the standard screen.
 */
int
scanw(char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    int rv = _sscans(stdscr, fmt, argp);
    va_end(argp);

    return rv;
}

/*
 * This routine implements a scanf on the given window.
 */
int
wscanw(WINDOW *win, char *fmt, ...)
{
    if (win == NULL)
        return ERR;

    va_list argp;
    va_start(argp, fmt);
    int rv = _sscans(win, fmt, argp);
    va_end(argp);

    return rv;
}

#ifdef atarist

/* modelled on the code from the new GCC library (originally from Dale
 * Schumacher's dLibs library).
 */

static int
sgetc(s)
unsigned char **s;
{
    register unsigned char c;

    c = *(*s)++;
    return ((c == '\0') ? EOF : c);
}

static int
sungetc(c, s)
int c;
unsigned char **s;
{
    if (c == EOF)
        c = '\0';
    return (*--(*s) = c);
}

#ifdef __STDC__
int
_sscans(WINDOW *win, char *fmt, ...)
#else
int
_sscans(win, fmt)
WINDOW *win;
char *fmt;
#endif
{
    char buf[128], *junk;
    extern int _scanf();
    va_list argp;

    if (wgetstr(win, buf) < 0)
        return ERR;
    va_start(argp, fmt);
    junk = buf;
    return (_scanf(&junk, sgetc, sungetc, fmt, argp));
}

#else                           /* original BSD routines */

/*
 * This routine actually executes the scanf from the window.
 *
 * This is really a modified version of "sscanf".  As such,
 * it assumes that sscanf interfaces with the other scanf functions
 * in a certain way.  If this is not how your system works, you
 * will have to modify this routine to use the interface that your
 * "sscanf" uses.
 */
#ifdef __STDC__
int
_sscans(win, fmt, ...)
#else
int
_sscans(win, fmt)
WINDOW *win;
char *fmt;
#endif
{

    char buf[100];
    FILE junk;
    va_list argp;

    junk._flag = _IOREAD | _IOSTRG;
    junk._base = junk._ptr = buf;
    if (wgetstr(win, buf) == ERR)
        return ERR;
    va_start(argp, fmt);
    junk._cnt = strlen(buf);
    return _doscan(&junk, fmt, argp);
}
#endif                          /* atarist */
