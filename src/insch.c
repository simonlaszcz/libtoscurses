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
 * This routine performs an insert-char on the line, leaving
 * (_cury,_curx) unchanged.
 *
 */
int
winsch(WINDOW *win, chtype c)
{
    tracev1("win=%p, c=%ld", win, c);

    if (win == NULL)
        return ERR;

    chtype *end = &(win->_y[win->_cury][win->_curx]);
    chtype *temp1 = &(win->_y[win->_cury][win->_maxx - 1]);
    chtype *temp2 = temp1 - sizeof(chtype);

    while (temp1 > end)
        *temp1-- = *temp2--;
    *temp1 = c;

    touchline(win, win->_cury, 1);

    return OK;
}
