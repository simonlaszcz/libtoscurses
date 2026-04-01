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
#include <stdarg.h>

static int do_wscanw(WINDOW *win, char *fmt, va_list argp);

int
wscanw(WINDOW *win, char *fmt, ...)
{
    tracev1("win=%p", win);

    if (win == NULL)
        return ERR;

    va_list argp;
    va_start(argp, fmt);
    int rv = do_wscanw(win, fmt, argp);
    va_end(argp);

    return rv;
}

int
mvwscanw(WINDOW *win, int y, int x, char *fmt, ...)
{
    tracev1("win=%p, y=%d, x=%d", win, y, x);

    if (win == NULL)
        return ERR;

    int rv = ERR;

    if (wmove(win, y, x) == OK) {
        va_list argp;
        va_start(argp, fmt);
        rv = do_wscanw(win, fmt, argp);
        va_end(argp);
    }

    return rv;
}

static int
do_wscanw(WINDOW *win, char *fmt, va_list argp)
{
    char buf[128];

    //TODO: need wngetstr
    if (wgetstr(win, buf) < 0)
        return ERR;

    int rv = vsscanf(buf, fmt, argp);

    if (rv == EOF)
        rv = ERR;

    return rv;
}
