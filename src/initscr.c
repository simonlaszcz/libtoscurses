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
#include <xyzst/xyz.h>
#include "vt52.h"

/*
 * This routine initializes the current and standard screen.
 *
 */
WINDOW *
initscr(void)
{
#ifdef DEBUG
    xyz_trace_file = fopen("ctrace.txt", "wt");
#endif

    struct xyz_con_info con;
    if (xyz_get_con_info(&con) == XYZ_OK) {
        LINES = con.lines;
        COLS = con.cols;
    }
    else {
        LINES = 25;
        COLS = 80;
    }
    SANE();

    if (curscr != NULL && delwin(curscr) == ERR)
        abend("Failed to delete curscr");
    if ((curscr = newwin(LINES, COLS, 0, 0)) == NULL)
        abend("Failed to create curscr");

    clearok(curscr, TRUE);
    curscr->_flags &= ~_FULLLINE;

    if (stdscr != NULL && delwin(stdscr) == ERR)
        abend("Failed to delete stdscr");
    if ((stdscr = newwin(LINES, COLS, 0, 0)) == NULL)
        abend("Failed to create stdscr");

    init_getch();
    init_color();

    return stdscr;
}
