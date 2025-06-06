#include "internal.h"
#include <string.h>

int
wscrl(WINDOW *win, int n)
{
    tracev1("win=%p, n=%d", win, n);

    if (win == NULL)
        return ERR;

    return do_scroll(win, win->_regtop, win->_regbottom, n);
}

int
do_scroll(WINDOW *win, int top, int bot, int n)
{
    tracev1("win=%p, top=%d, bottom=%d, n=%d", win, top, bot, n);

    if (win == NULL || !win->_scroll)
        return ERR;
    if (top < 0 || top >= win->_maxy || bot < 0 || bot >= win->_maxy || top >= bot)
        return ERR;
    if (n == 0)
        return OK;

    int lnsz = win->_maxx * sizeof(chtype);

    if (n < 0) {
        /* scroll down n lines */
        int limit = top - n;
        for (int y = bot; y >= limit && y >= 0 && y >= top; --y)
            memcpy(win->_y[y], win->_y[y + n], lnsz);
        for (int y = top; y < limit && y < win->_maxy && y <= bot; ++y)
            for (int x = 0; x < win->_maxx; x++)
                win->_y[y][x] = win->_bkgd;
    }

    if (n > 0) {
        /* scroll up n lines */
        int limit = bot - n;
        for (int y = top; y <= limit && y < win->_maxy && y <= bot; ++y)
            memcpy(win->_y[y], win->_y[y + n], lnsz);
        for (int y = bot; y > limit && y >= 0 && y >= top; --y)
            for (int x = 0; x < win->_maxx; ++x)
                win->_y[y][x] = win->_bkgd;
    }

    if (touchline(win, top, bot - top + 1) == ERR)
        return ERR;

    return OK;
}
