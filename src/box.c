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
 * This routine draws a box around the given window with "vert"
 * as the vertical delimiting char, and "hor", as the horizontal one.
 *
 */
int
box(WINDOW *win, chtype vert, chtype hor)
{
    tracev1("win=%p, vert=%ld, hor=%ld", win, vert, hor);

    if (win == NULL)
        return ERR;

    if (vert == 0)
        vert = ACS_VLINE;
    if (hor == 0)
        hor = ACS_HLINE;

    int endx = win->_maxx;
    int endy = win->_maxy - 1;
    chtype *fp = win->_y[0];
    chtype *lp = win->_y[endy];

    for (int i = 0; i < endx; i++)
        fp[i] = lp[i] = hor;
    endx--;
    for (int i = 0; i <= endy; i++)
        win->_y[i][0] = (win->_y[i][endx] = vert);
    if (!win->_scroll && (win->_flags & _SCROLLWIN))
        fp[0] = fp[endx] = lp[0] = lp[endx] = win->_bkgd;

    return touchwin(win);
}
