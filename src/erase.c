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
 * This routine erases everything on the window.
 *
 */
int
werase(WINDOW *win)
{
    tracev1("win=%p", win);

    if (win == NULL)
        return ERR;

    for (int y = 0; y < win->_maxy; ++y)
        for (int x = 0; x < win->_maxx; ++x)
            win->_y[y][x] = win->_bkgd;

    win->_curx = win->_cury = 0;

    return touchwin(win);
}
