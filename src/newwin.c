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
#include <stdlib.h>

static WINDOW *makenew(int num_lines, int num_cols, int begy, int begx);

/*
 * allocate space for and set up defaults for a new window
 *
 */
WINDOW *
newwin(int num_lines, int num_cols, int begy, int begx)
{
    tracev1("lines=%d, cols=%d, y=%d, x=%d", num_lines, num_cols, begy, begx);
    WINDOW *win = NULL;

    if (num_lines == 0)
        num_lines = LINES - begy;
    if (num_cols == 0)
        num_cols = COLS - begx;

    if ((win = makenew(num_lines, num_cols, begy, begx)) == NULL)
        goto ret_null;
    if ((win->_firstch = malloc(num_lines * sizeof(win->_firstch[0]))) == NULL)
        goto ret_null;
    if ((win->_lastch = malloc(num_lines * sizeof(win->_lastch[0]))) == NULL)
        goto ret_null;

    win->_nextp = win;

    for (int i = 0; i < num_lines; i++) {
        win->_firstch[i] = _NOCHANGE;
        win->_lastch[i] = _NOCHANGE;
    }

    for (int i = 0; i < num_lines; i++) {
        if ((win->_y[i] = malloc(num_cols * sizeof(win->_y[0]))) == NULL)
            goto ret_null;
        for (int x = 0; x < num_cols; ++x)
            win->_y[i][x] = ' ';
    }

    win->_ch_off = 0;

    return win;

ret_null:
    trace1("malloc failed");
    if (win != NULL) {
        for (int j = 0; j < num_lines; j++)
            free(win->_y[j]);
        free(win->_y);
        free(win->_firstch);
        free(win->_lastch);
        free(win);
    }
    return NULL;
}

WINDOW *
subwin(WINDOW *orig, int num_lines, int num_cols, int begy, int begx)
{
    tracev1("orig=%p, num_lines=%d, num_cols=%d, begy=%d, begx=%d",
        orig, num_lines, num_cols, begy, begx);
    WINDOW *win = NULL;

    /*
     * make sure window fits inside the original one
     */
    if (orig == NULL || begy < orig->_begy || begx < orig->_begx
        || begy + num_lines > orig->_maxy + orig->_begy
        || begx + num_cols > orig->_maxx + orig->_begx)
        return NULL;

    if (num_lines == 0)
        num_lines = orig->_maxy + orig->_begy - begy;
    if (num_cols == 0)
        num_cols = orig->_maxx + orig->_begx - begx;

    if ((win = makenew(num_lines, num_cols, begy, begx)) == NULL)
        return NULL;

    win->_nextp = orig->_nextp;
    orig->_nextp = win;
    win->_orig = orig;
    _set_subwin_(orig, win);

    return win;
}

/*
 * This routine sets up a window buffer and returns a pointer to it.
 */
static WINDOW *
makenew(int num_lines, int num_cols, int begy, int begx)
{
    tracev1("num_lines=%d, num_cols=%d, begy=%d, begx=%d", num_lines, num_cols, begy, begx);
    WINDOW *win = NULL;

    if ((win = malloc(sizeof(*win))) == NULL)
        goto ret_null;

    memset(win, 0, sizeof(*win));
    win->_maxy = num_lines;
    win->_maxx = num_cols;
    win->_begy = begy;
    win->_begx = begx;
    win->_regtop = 0;
    win->_regbottom = num_lines - 1;
    win->_bkgd = ' ' | A_NORMAL;

    /* just allocating an array of pointers to each line */
    if ((win->_y = malloc(num_lines * sizeof(win->_y[0]))) == NULL)
        goto ret_null;

    _swflags_(win);
    tracev1("clear=%d, leave=%d, scroll=%d, flags=%d, maxy=%d, max=%d, begy=%d, begx=%d",
            win->_clear, win->_leave, win->_scroll, win->_flags, win->_maxy, win->_maxx, win->_begy,
            win->_begx);

    return win;

ret_null:
    trace1("malloc failed");
    if (win != NULL)
        free(win->_y);
    free(win);
    return NULL;
}

/*
 * this code is shared with mvwin()
 */
void
_set_subwin_(WINDOW *orig, WINDOW *win)
{
    tracev1("orig=%p, win=%p", orig, win);
    int j = win->_begy - orig->_begy;
    int k = win->_begx - orig->_begx;
    win->_ch_off = k;
    tracev1("_ch_off=%d", k);

    win->_firstch = &orig->_firstch[j];
    win->_lastch = &orig->_lastch[j];

    /* n.b. data shared from parent */
    for (int i = 0; i < win->_maxy; i++, j++)
        win->_y[i] = &orig->_y[j][k];
}

void
_swflags_(WINDOW *win)
{
    tracev1("win=%p", win);
    win->_flags &= ~(_ENDLINE | _FULLLINE | _FULLWIN | _SCROLLWIN);

    if (win->_begx + win->_maxx == COLS) {
        win->_flags |= _ENDLINE;
        if (win->_begx == 0) {
            win->_flags |= _FULLLINE;
            if (win->_maxy == LINES && win->_begy == 0)
                win->_flags |= _FULLWIN;
        }
        if (win->_begy + win->_maxy == LINES)
            win->_flags |= _SCROLLWIN;
    }
}
