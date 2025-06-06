#include "internal.h"
#include <stdio.h>

int
scr_dump(const char *filename)
{
    if (filename == NULL || curscr == NULL)
        return ERR;

    FILE *fout = NULL;

    if ((fout = fopen(filename, "wb")) == NULL)
        return ERR;
    for (int y = 0; y < curscr->_maxy; ++y)
        fwrite(curscr->_y[y], sizeof(chtype), curscr->_maxx, fout);

    return fclose(fout) == 0 ? OK : ERR;
}
