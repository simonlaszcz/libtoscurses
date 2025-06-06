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

static int do_newline(WINDOW *win);
static chtype attrify(WINDOW *win, chtype ch);

/**
 * This routine adds the character to the current position
 */
int
waddch(WINDOW *win, const chtype c)
{
    if (win == NULL)
        return ERR;

    short *x = &(win->_curx);
    short *y = &(win->_cury);
    chtype ch = c;

    switch (ch & A_CHARTEXT) {
    case '\t':
        chtype blank = ' ' | (ch & A_ATTRIBUTES);
        int ntabs = TABSIZE-((*x)%TABSIZE);
        for (int tc = 0; tc < ntabs; ++tc)
            if (waddch(win, blank) == ERR)
                return ERR;
        return OK;
    case '\n':
        if (wclrtoeol(win) == ERR)
            return ERR;
        return do_newline(win);	
    case '\r':
        *x = 0;
        break;
    case '\b':
        if (--(*x) < 0)
            *x = 0;
        break;
    default:
        ch = attrify(win, ch);
        tracev2("win=%p, y=%02d, x=%02d, attr=%04x, color=%02x, ch=%c",
            win, *y, *x, TRATTR(ch), TRCOLOR(ch), TRCHAR(ch));

        if (win->_y[*y][*x] != ch) {
            win->_y[*y][*x] = ch;
            int newx = *x + win->_ch_off;

            if (win->_firstch[*y] == _NOCHANGE || win->_lastch[*y] == _NOCHANGE)
                win->_firstch[*y] = win->_lastch[*y] = newx;
            else if (newx < win->_firstch[*y])
                win->_firstch[*y] = newx;
            else if (newx > win->_lastch[*y])
                win->_lastch[*y] = newx;
        }

        if (*x < win->_maxx - 1)
            ++(*x);
        else
            return do_newline(win);	
        break;
    }

    return OK;
}

static int
do_newline(WINDOW *win)
{
    short *y = &(win->_cury);
    short *x = &(win->_curx);

    *x = 0;

    if (*y >= win->_regtop && *y <= win->_regbottom)
        if (*y == win->_regbottom)
            return do_scroll(win, win->_regtop, win->_regbottom, 1);

    if (*y < win->_maxy - 1)
        ++(*y);

    return OK;
}

#define COLOR_MASK(ch) (~(attr_t)(((ch) & A_COLOR) ? A_COLOR : 0))

static inline chtype
attrify(WINDOW *win, chtype ch)
{
    int pair = PAIR_NUMBER(ch);
    attr_t a = win->_attrs & A_ATTRIBUTES;

    if ((ch & A_CHARTEXT) == ' ' && (ch & A_ATTRIBUTES) == A_NORMAL && pair == 0) {
        ch = win->_bkgd;
        ch |= (a | (win->_bkgd & A_ATTRIBUTES));
        if ((pair = PAIR_NUMBER(win->_attrs)) == 0)
            pair = PAIR_NUMBER(win->_bkgd);
    }
    else {
        a |= ((win->_bkgd & A_ATTRIBUTES) & COLOR_MASK(a));
        if (pair == 0)
            if ((pair = PAIR_NUMBER(win->_attrs)) == 0)
                pair = PAIR_NUMBER(win->_bkgd);
        ch |= (a & COLOR_MASK(ch & A_ATTRIBUTES));
    }

    ch &= COLOR_MASK(ch);
    ch |= COLOR_PAIR(pair);

    return ch;
}
