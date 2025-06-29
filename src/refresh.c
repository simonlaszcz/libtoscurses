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

/*
 * make the current screen look like "win" over the area coverd by
 * win.
 */

#include "internal.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "vt52.h"

static int change_line(WINDOW *, short);
static void start_color_pair(short pair, bool init);
static void set_reverse(bool bf, bool init);

static short ly, lx;
static bool is_curscr;
static bool was_cleared;

int
wrefresh(WINDOW *win)
{
    tracev1("win=%p", win);

    if (win == NULL)
        return ERR;

    ly = curscr->_cury;
    lx = curscr->_curx;
    is_curscr = (win == curscr);
    was_cleared = false;

#ifndef TOSCOMPAT
    if (win->_flags & _FULLWIN)
        sync_bg(win);
#endif

    if (win->_clear || curscr->_clear || is_curscr) {
        if ((win->_flags & _FULLWIN) || curscr->_clear) {
            BG(screen_bg_tos_color());
            CLS();
            ly = 0;
            lx = 0;
            was_cleared = true;
            if (!is_curscr) {
                curscr->_clear = false;
                werase(curscr);
            }
            touchwin(win);
        }
        win->_clear = false;
    }

    WRAP(true);
    int cursor = curs_set(0);

    for (int wy = 0; wy < win->_maxy; wy++) {
        if (win->_firstch[wy] != _NOCHANGE) {
            (void)change_line(win, wy);
            if (win->_firstch[wy] >= win->_ch_off)
                win->_firstch[wy] = win->_maxx + win->_ch_off;
            if (win->_lastch[wy] < win->_maxx + win->_ch_off)
                win->_lastch[wy] = win->_ch_off;
            if (win->_lastch[wy] < win->_firstch[wy])
                win->_firstch[wy] = _NOCHANGE;
        }
    }

    curs_set(cursor);

    if (is_curscr) {
        mvcur(-1, -1, win->_cury, win->_curx);
    }
    else {
        if (win->_leave) {
            curscr->_cury = ly;
            curscr->_curx = lx;
            ly -= win->_begy;
            lx -= win->_begx;
            if (ly >= 0 && ly < win->_maxy && lx >= 0 && lx < win->_maxx) {
                win->_cury = ly;
                win->_curx = lx;
            }
            else {
                win->_cury = win->_curx = 0;
            }
        }
        else {
            curscr->_cury = win->_cury + win->_begy;
            curscr->_curx = win->_curx + win->_begx;
            mvcur(-1, -1, curscr->_cury, curscr->_curx);
        }
    }

    fflush(stdout);

    return OK;
}

static void
start_color_pair(short pair, bool init)
{
    static short cf = 0, cb = 0;
    short f, b;

    if (tos_pair_content(pair, &f, &b) == ERR)
        return;
    if (!init && cf == f && cb == b)
        return;

    FG(f);
    BG(b);

    cf = f;
    cb = b;
    tracev1("curscr f=%d, b=%d", f, b);
}

static void
set_reverse(bool bf, bool init)
{
    static bool cbf = false;

    if (!init && cbf == bf)
        return;

    REVERSE(bf);
    cbf = bf;
    tracev1("curscr reverse=%d", bf);
}

static inline bool
is_bg_transparent(chtype ch, short sys_bg)
{
    if ((ch & A_CHARTEXT) == ' ' && ((ch & ~A_COLOR) & A_ATTRIBUTES) == 0) {
        short f, b = sys_bg;

        if (ch & A_COLOR) {
            int pn = PAIR_NUMBER(ch);
            if (pair_content(pn, &f, &b) == ERR)
                b = sys_bg;
        }

        return b == sys_bg;
    }

    return false;
}

/*
 * make a change on the screen
 */
static int
change_line(WINDOW *win, short wy)
{
    chtype dummy[] = {0, 0};

    int wx = win->_firstch[wy] - win->_ch_off;
    if (wx >= win->_maxx)
        return OK;
    else if (wx < 0)
        wx = 0;
    int lch = win->_lastch[wy] - win->_ch_off;
    if (lch < 0)
        return OK;
    else if (lch >= win->_maxx)
        lch = win->_maxx - 1;
    int screeny = wy + win->_begy;
    tracev1("win=%p, wy=%d, screeny=%d, wx=%d, lch=%d", win, wy, screeny, wx, lch);

    if ((win->_flags & _ENDLINE) && !is_curscr) {
        /* mvcur and clear takes 11 bytes so only clear if it's cost effective
           allow 2 bytes leeway since we've already done the preprocessing */
        const int cost_threshold = 8;
        chtype *ep = &(win->_y[wy][win->_maxx - 1]);
        chtype last = *ep;
        int eol_blanks_startx = win->_maxx;
        short sys_bg = screen_bg_color();

        /* find the range of spaces with sys bg from eol */
        while (is_bg_transparent(*ep--, sys_bg) && eol_blanks_startx >= 0)
            --eol_blanks_startx;
        if (eol_blanks_startx < 0)
            eol_blanks_startx = 0;
        tracev1("win=%p, wy=%d, eol_blanks_startx=%d, wx=%d, lch=%d, was_cleared=%d",
            win, wy, eol_blanks_startx, wx, lch, was_cleared);

        if (eol_blanks_startx <= wx) {
            /* full row of blanks OR overwriting with blanks at eol */
            tracev1("win=%p, wy=%d, eol_blanks_startx=%d, wx=%d, lch=%d, was_cleared=%d CLEARING FULL EOL",
                win, wy, eol_blanks_startx, wx, lch, was_cleared);
            if (!was_cleared) {
                mvcur(-1, -1, screeny, eol_blanks_startx + win->_begx);
                BG(screen_bg_tos_color());
                REVERSE(false);
                CLREOL();
            }
            for (int x = eol_blanks_startx; x < win->_maxx; ++x)
                curscr->_y[screeny][x + win->_begx] = last;
            return OK;
        }

        int span = lch - eol_blanks_startx;

        if (span > cost_threshold || (span > 0 && was_cleared)) {
            /* writing text with trailing blanks */
            tracev1("win=%p, wy=%d, eol_blanks_startx=%d, wx=%d, lch=%d, was_cleared=%d CLEARING EOL",
                win, wy, eol_blanks_startx, wx, lch, was_cleared);
            if (!was_cleared) {
                mvcur(-1, -1, screeny, eol_blanks_startx + win->_begx);
                BG(screen_bg_tos_color());
                REVERSE(false);
                CLREOL();
            }
            for (int x = eol_blanks_startx; x < win->_maxx; ++x)
                curscr->_y[screeny][x + win->_begx] = last;
            lch = eol_blanks_startx - 1;
        }
#if 0
        else {
            /* check for writing text with leading blanks */
            chtype *sp = &(win->_y[wy][0]);
            chtype first = *sp;
            int sol_blanks_endx = -1;

            while (is_bg_transparent(*sp++, sys_bg) && sol_blanks_endx < win->_maxx)
                ++sol_blanks_endx;
            /* won't happen as a full line of blanks is handled above */
            if (sol_blanks_endx >= win->_maxx)
                sol_blanks_endx = win->_maxx - 1;
            tracev1("win=%p, wy=%d, sol_blanks_endx=%d, wx=%d, lch=%d, was_cleared=%d",
                win, wy, sol_blanks_endx, wx, lch, was_cleared);

            span = sol_blanks_endx - wx;

            if (span > cost_threshold || (span > 0 && was_cleared)) {
                tracev1("win=%p, wy=%d, sol_blanks_endx=%d, wx=%d, lch=%d, was_cleared=%d CLEARING SOL",
                    win, wy, sol_blanks_endx, wx, lch, was_cleared);
                if (!was_cleared) {
                    mvcur(-1, -1, screeny, sol_blanks_endx + win->_begx);
                    BG(screen_bg_tos_color());
                    REVERSE(false);
                    CLRSOL();
                }
                for (int x = 0; x <= sol_blanks_endx; ++x)
                    curscr->_y[screeny][x + win->_begx] = first;
                wx = sol_blanks_endx + 1;
            }
        }
#endif
    }

    /* point to curscr, or to a dummy buffer if win is curscr.
       if pointing to dummy, ensure csp is never incremented! */
    chtype *csp = dummy;
    if (!is_curscr)
        csp = &(curscr->_y[screeny][wx + win->_begx]);
    chtype *nsp = &(win->_y[wy][wx]);
    tracev1("win=%p, wy=%d, wx=%d, lch=%d", win, wy, wx, lch);

    while (wx <= lch) {
        if (*nsp != *csp) {
            start_color_pair(PAIR_NUMBER(*csp), true);
            set_reverse(*csp & A_REVERSE, true);
            mvcur(-1, -1, screeny, wx + win->_begx);
            ly = screeny;
            lx = wx + win->_begx;

            while (*nsp != *csp && wx <= lch) {
                start_color_pair(PAIR_NUMBER(*nsp), false);
                set_reverse(*nsp & A_REVERSE, false);

                /* check if physical screen would scroll */
                wx++;
                if (wx >= win->_maxx && wy == win->_maxy - 1) {
                    if (win->_scroll) {
                        if (!is_curscr)
                            PUT((*csp = *nsp) & A_CHARTEXT);
                        else
                            PUT(*nsp & A_CHARTEXT);
                        if ((win->_flags & _FULLWIN) && !is_curscr)
                            scroll(curscr);
                        ly = win->_begy + win->_cury;
                        lx = win->_begx + win->_curx;
                    }
                    else if (win->_flags & _SCROLLWIN) {
                        lx = --wx;
                        WRAP(false);
                        if (!is_curscr)
                            PUT((*csp = *nsp) & A_CHARTEXT);
                        else
                            PUT(*nsp & A_CHARTEXT);
                    }

                    return OK;
                }

                if (!is_curscr)
                    PUT((*csp++ = *nsp) & A_CHARTEXT);
                else
                    PUT(*nsp & A_CHARTEXT);
                nsp++;
            }                   /* while (*nsp != *csp && wx <= lch) */

            if (lx == wx + win->_begx)  /* if no change */
                break;
            lx = wx + win->_begx;
            if (lx >= COLS) {
                lx = 0;
                ly++;
            }
        }                       /* if (*nsp != *csp) */
        else if (wx <= lch) {
            while (*nsp == *csp && wx <= lch) {
                nsp++;
                if (!is_curscr)
                    csp++;
                ++wx;
            }
        }
        else {
            break;
        }
    }

    return OK;
}
