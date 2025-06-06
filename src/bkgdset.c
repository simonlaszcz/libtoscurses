#include "internal.h"

void
wbkgdset(WINDOW *win, chtype ch)
{
    tracev1("win=%p, ch=%ld", win, ch);
    tracev2("win=%p, bkgd_attr=%04x, bkgd_color=%02x, bkgd_ch=%c",
        win, TRATTR(win->_bkgd), TRCOLOR(win->_bkgd), TRCHAR(win->_bkgd));
    tracev2("win=%p, win_attr=%04x, win_color=%02x, win_ch=%c",
        win, TRATTR(win->_attrs), TRCOLOR(win->_attrs), TRCHAR(win->_attrs));

    if (win == NULL)
        return;

    attr_t off = win->_bkgd & A_ATTRIBUTES;
    attr_t on = ch & A_ATTRIBUTES;

    toggle_attr_off(win->_attrs, off);
    toggle_attr_on(win->_attrs, on);

    if ((ch & A_CHARTEXT) == 0)
        win->_bkgd = ' ' | (ch & A_ATTRIBUTES);
    else
        win->_bkgd = ch;

    tracev2("win=%p, bkgd_attr=%04x, bkgd_color=%02x, bkgd_ch=%c",
        win, TRATTR(win->_bkgd), TRCOLOR(win->_bkgd), TRCHAR(win->_bkgd));
    tracev2("win=%p, win_attr=%04x, win_color=%02x, win_ch=%c",
        win, TRATTR(win->_attrs), TRCOLOR(win->_attrs), TRCHAR(win->_attrs));
}
