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

#include <toscurses/curses.h>

WINDOW *stdscr = NULL;
WINDOW *curscr = NULL;

bool _echoit = true;            /* set if stty indicates ECHO */
bool _rawmode = false;          /* set if stty indicates RAW mode */
bool _pfast = false;
long _ttyflags = 0;

int LINES;                      /* number of lines allowed on screen    */
int COLS;                       /* number of columns allowed on screen  */
int COLORS;
int COLOR_PAIRS;
int ESCDELAY = 0;
int TABSIZE = 8;

#ifdef DEBUG
/* debug output file */
FILE *xyz_trace_file = NULL;
#endif
