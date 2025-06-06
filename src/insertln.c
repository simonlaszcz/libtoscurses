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
 *	This routine performs an insert-line on the window, leaving
 * (_cury,_curx) unchanged.
 *
 */
int
winsertln(WINDOW *win)
{
    tracev1("win=%p", win);

    if (win == NULL)
        return ERR;

    chtype *temp = win->_y[win->_maxy - 1];
    int	y = 0;

    for (y = win->_maxy - 1; y > win->_cury; --y) {
        if (win->_orig == NULL)
            win->_y[y] = win->_y[y - 1];
        else
            bcopy(win->_y[y - 1], win->_y[y], win->_maxx);
    }

    if (win->_orig == NULL)
        win->_y[y] = temp;
    else
        temp = win->_y[y];

    for (chtype *end = &temp[win->_maxx]; temp < end; )
        *temp++ = win->_bkgd;

    if (touchline(win, win->_cury, win->_maxy - win->_cury) == ERR)
        return ERR;

    if (win->_orig == NULL)
        _id_subwins(win);

    return OK;
}
