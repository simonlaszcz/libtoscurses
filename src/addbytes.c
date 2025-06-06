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
 * This routine adds the character to the current position
 */
int
waddbytes(WINDOW *win, const char *bytes, int count)
{
    if (win == NULL || bytes == NULL || count < -1)
        return ERR;

    while ((count == -1 || count--) && *bytes != NULLCH) {
        chtype c = *bytes++ | A_NORMAL;

        if (waddch(win, c) == ERR)
            return ERR;
    }

    return OK;
}
