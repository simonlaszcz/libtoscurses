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

/**
 * This routine clears to the end of line from the cursor
 */
int
wclrtoeol(WINDOW *win)
{
    tracev1("win=%p", win);

    if (win == NULL)
        return ERR;

    chtype *sp = &(win->_y[win->_cury][win->_curx]);
    chtype *ep = &(win->_y[win->_cury][win->_maxx]);
    tracev1("win=%p, cury=%d, curx=%d", win, win->_cury, win->_curx);

    while (sp < ep)
        *sp++ = win->_bkgd;

    int chidx = win->_maxx - 1 + win->_ch_off;
    win->_lastch[win->_cury] = chidx;

    chidx = win->_curx + win->_ch_off;
    if (win->_firstch[win->_cury] == _NOCHANGE || chidx < win->_firstch[win->_cury])
        win->_firstch[win->_cury] = chidx; 

    tracev1("win=%p, firstch=%d, lastch=%d",
        win,
        win->_firstch[win->_cury] - win->_ch_off,
        win->_lastch[win->_cury] - win->_ch_off);

    return OK;
}
