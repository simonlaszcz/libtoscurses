/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Berkeley. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 *
 */

#ifndef INTERNAL_H
#define INTERNAL_H

#include <toscurses/curses.h>
#include <xyzst/trace.h>
#include "termcap.h"

#ifndef NULLCH
#define NULLCH '\0'
#endif

extern bool _echoit, _rawmode, _pfast;
extern int LINES, COLS, COLORS, COLOR_PAIRS, ESCDELAY, TABSIZE;
extern long _ttyflags;

#ifdef DEBUG
#define TRCOLOR(ch) ((char)((ch & A_COLOR) >> NCURSES_ATTR_SHIFT))
#define TRATTR(ch)  ((short)((ch & A_ATTRIBUTES) >> (NCURSES_ATTR_SHIFT + 8)))
#define TRCHAR(ch)	((char)(((chtype)ch) & A_CHARTEXT))
#define outf		xyz_trace_file
#endif

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

#define toggle_attr_on(S,at) {\
    if (PAIR_NUMBER(at) > 0) {\
        (S) = ((S) & ~A_COLOR) | (attr_t) (at);\
    } else {\
        (S) |= (attr_t) (at);\
    }}

#define toggle_attr_off(S,at) {\
    if (PAIR_NUMBER(at) > 0) {\
        (S) &= ~(at|A_COLOR);\
    } else {\
        (S) &= ~(at);\
    }}

__EXTERN void _id_subwins __PROTO((WINDOW *));
__EXTERN void _set_subwin_ __PROTO((WINDOW *, WINDOW *));
__EXTERN int _sprintw __PROTO((WINDOW *, char *, char *));
__EXTERN int _sscans __PROTO((WINDOW *, char *, ...));
__EXTERN void _swflags_ __PROTO((WINDOW *));

void init_getch(void);
void restore_getch(void);
void init_color(void);
void restore_color(void);
int do_scroll(WINDOW * win, int top, int bottom, int n);
int tos_pair_content(short pair, short *f, short *b);
void sync_bg(WINDOW * win);
short screen_bg_color(void);
short screen_bg_tos_color(void);

#endif
