#include "internal.h"

int
mvaddnstr(int y, int x, const char *str, int n)
{
    if (wmove(stdscr, y, x) == OK && waddbytes(stdscr, (char *)str, n) == OK)
        return OK;
    return ERR;
}
