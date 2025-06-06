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

/*
 * make it look like the whole window has been changed.
 *
 */
int
touchwin(WINDOW *win)
{
    tracev1("win=%p", win);
    if (win == NULL)
        return ERR;
    return touchline(win, 0, win->_maxy);
}

/*
 * touch a given line
 */
int
touchline(WINDOW *win, int start, int count)
{
    tracev1("win=%p, start=%d, count=%d", win, start, count);
    if (win == NULL)
        return ERR;

    if (start < 0)
        start = 0;
    int end = start + count;
    if (end > win->_maxy)
        end = win->_maxy;

    int sx = win->_ch_off;
    int ex = win->_ch_off + win->_maxx - 1;

    for (int y = start; y < end; ++y) {
        win->_firstch[y] = sx;
        win->_lastch[y] = ex;
    }

    return OK;
}
