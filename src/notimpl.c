#include "internal.h"

int
intrflush(UNUSED WINDOW *win, UNUSED bool bf)
{
    NOT_IMPLEMENTED("TOS Frotz doesn't get signals");
    return OK;
}

WINDOW *
newpad(UNUSED int nlines, UNUSED int ncols)
{
    NOT_IMPLEMENTED("Only used for screen resize which isn't handled by TOS Frotz");
    return NULL;
}

int
copywin(UNUSED const WINDOW *srcwin, UNUSED WINDOW *dstwin, UNUSED int sminrow, UNUSED int smincol, 
    UNUSED int dminrow, UNUSED int dmincol, UNUSED int dmaxrow, UNUSED int dmaxcol, UNUSED int overlay)
{
    NOT_IMPLEMENTED("Only used for screen resize which isn't handled by TOS Frotz");
    return ERR;
}
