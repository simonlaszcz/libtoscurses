#include "internal.h"

int
wsetscrreg(WINDOW *win, int top, int bot)
{
    tracev1("win=%p, top=%d, bottom=%d", win, top, bot);

    if (win == NULL)
        return ERR;
    if (top < 0 || top >= win->_maxy || bot < 0 || bot >= win->_maxy || top >= bot)
        return ERR;

    win->_regtop = top;
    win->_regbottom = bot;

    return OK;
}
