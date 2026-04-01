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
 *
 *	@(#)curses.h	5.4 (Berkeley) 6/30/88
 */

#ifndef WINDOW

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <sgtty.h>

/* attributes */
#define NCURSES_ATTR_SHIFT       8
#define NCURSES_CAST(type,value) (type)(value)
#define NCURSES_BITS(mask,shift) (NCURSES_CAST(chtype,(mask)) << ((shift) + NCURSES_ATTR_SHIFT))
#define COLOR_PAIR(n)	(NCURSES_BITS((n), 0) & A_COLOR)
#define PAIR_NUMBER(a)	(NCURSES_CAST(int,((NCURSES_CAST(unsigned long,(a)) & A_COLOR) >> NCURSES_ATTR_SHIFT)))

#define A_NORMAL		(1UL - 1UL)
#define A_ATTRIBUTES	NCURSES_BITS(~(1UL - 1UL),0)
#define A_CHARTEXT		(NCURSES_BITS(1UL,0) - 1UL)
#define A_COLOR			NCURSES_BITS(((1UL) << 8) - 1UL,0)
#define A_STANDOUT		NCURSES_BITS(1UL,8)
#define A_UNDERLINE		NCURSES_BITS(1UL,9)
#define A_REVERSE		NCURSES_BITS(1UL,10)
#define A_BLINK			NCURSES_BITS(1UL,11)
#define A_DIM			NCURSES_BITS(1UL,12)
#define A_BOLD			NCURSES_BITS(1UL,13)
#define A_ALTCHARSET	NCURSES_BITS(1UL,14)
#define A_INVIS			NCURSES_BITS(1UL,15)
#define A_PROTECT		NCURSES_BITS(1UL,16)
#define A_HORIZONTAL	NCURSES_BITS(1UL,17)
#define A_LEFT			NCURSES_BITS(1UL,18)
#define A_LOW			NCURSES_BITS(1UL,19)
#define A_RIGHT			NCURSES_BITS(1UL,20)
#define A_TOP			NCURSES_BITS(1UL,21)
#define A_VERTICAL		NCURSES_BITS(1UL,22)

/* colors */
#define COLOR_BLACK		0
#define COLOR_RED		1
#define COLOR_GREEN		2
#define COLOR_YELLOW	3
#define COLOR_BLUE		4
#define COLOR_MAGENTA	5
#define COLOR_CYAN		6
#define COLOR_WHITE		7

#define ACS_PLUS        '+'
#define ACS_LTEE        '<'
#define ACS_RTEE        '>'
#define ACS_TTEE        '^'
#define ACS_BTEE        'v'
#define ACS_HLINE       '-'
#define ACS_VLINE       '|'
#define ACS_CKBOARD     ':'
#define ACS_URCORNER    '+'
#define ACS_ULCORNER    '+'
#define ACS_LLCORNER    '+'
#define ACS_LRCORNER    '+'

/* keys */
#define KEY_DOWN		0402		/* down-arrow key */
#define KEY_UP			0403		/* up-arrow key */
#define KEY_LEFT		0404		/* left-arrow key */
#define KEY_RIGHT		0405		/* right-arrow key */
#define KEY_HOME		0406		/* home key */
#define KEY_BACKSPACE	0407		/* backspace key */
#define KEY_F0			0410		/* Function keys.  Space for 64 */
#define KEY_F(n)		(KEY_F0+(n))	/* Value of function key n */
#define KEY_DL			0510		/* delete-line key */
#define KEY_IL			0511		/* insert-line key */
#define KEY_DC			0512		/* delete-character key */
#define KEY_IC			0513		/* insert-character key */
#define KEY_EIC			0514		/* sent by rmir or smir in insert mode */
#define KEY_CLEAR		0515		/* clear-screen or erase key */
#define KEY_EOS			0516		/* clear-to-end-of-screen key */
#define KEY_EOL			0517		/* clear-to-end-of-line key */
#define KEY_SF			0520		/* scroll-forward key */
#define KEY_SR			0521		/* scroll-backward key */
#define KEY_NPAGE		0522		/* next-page key */
#define KEY_PPAGE		0523		/* previous-page key */
#define KEY_STAB		0524		/* set-tab key */
#define KEY_CTAB		0525		/* clear-tab key */
#define KEY_CATAB		0526		/* clear-all-tabs key */
#define KEY_ENTER		0527		/* enter/send key */
#define KEY_PRINT		0532		/* print key */
#define KEY_LL			0533		/* lower-left key (home down) */
#define KEY_A1			0534		/* upper left of keypad */
#define KEY_A3			0535		/* upper right of keypad */
#define KEY_B2			0536		/* center of keypad */
#define KEY_C1			0537		/* lower left of keypad */
#define KEY_C3			0540		/* lower right of keypad */
#define KEY_BTAB		0541		/* back-tab key */
#define KEY_BEG			0542		/* begin key */
#define KEY_CANCEL		0543		/* cancel key */
#define KEY_CLOSE		0544		/* close key */
#define KEY_COMMAND		0545		/* command key */
#define KEY_COPY		0546		/* copy key */
#define KEY_CREATE		0547		/* create key */
#define KEY_END			0550		/* end key */
#define KEY_EXIT		0551		/* exit key */
#define KEY_FIND		0552		/* find key */
#define KEY_HELP		0553		/* help key */
#define KEY_MARK		0554		/* mark key */
#define KEY_MESSAGE		0555		/* message key */
#define KEY_MOVE		0556		/* move key */
#define KEY_NEXT		0557		/* next key */
#define KEY_OPEN		0560		/* open key */
#define KEY_OPTIONS		0561		/* options key */
#define KEY_PREVIOUS	0562		/* previous key */
#define KEY_REDO		0563		/* redo key */
#define KEY_REFERENCE	0564		/* reference key */
#define KEY_REFRESH		0565		/* refresh key */
#define KEY_REPLACE		0566		/* replace key */
#define KEY_RESTART		0567		/* restart key */
#define KEY_RESUME		0570		/* resume key */
#define KEY_SAVE		0571		/* save key */
#define KEY_SBEG		0572		/* shifted begin key */
#define KEY_SCANCEL		0573		/* shifted cancel key */
#define KEY_SCOMMAND	0574		/* shifted command key */
#define KEY_SCOPY		0575		/* shifted copy key */
#define KEY_SCREATE		0576		/* shifted create key */
#define KEY_SDC			0577		/* shifted delete-character key */
#define KEY_SDL			0600		/* shifted delete-line key */
#define KEY_SELECT		0601		/* select key */
#define KEY_SEND		0602		/* shifted end key */
#define KEY_SEOL		0603		/* shifted clear-to-end-of-line key */
#define KEY_SEXIT		0604		/* shifted exit key */
#define KEY_SFIND		0605		/* shifted find key */
#define KEY_SHELP		0606		/* shifted help key */
#define KEY_SHOME		0607		/* shifted home key */
#define KEY_SIC			0610		/* shifted insert-character key */
#define KEY_SLEFT		0611		/* shifted left-arrow key */
#define KEY_SMESSAGE	0612		/* shifted message key */
#define KEY_SMOVE		0613		/* shifted move key */
#define KEY_SNEXT		0614		/* shifted next key */
#define KEY_SOPTIONS	0615		/* shifted options key */
#define KEY_SPREVIOUS	0616		/* shifted previous key */
#define KEY_SPRINT		0617		/* shifted print key */
#define KEY_SREDO		0620		/* shifted redo key */
#define KEY_SREPLACE	0621		/* shifted replace key */
#define KEY_SRIGHT		0622		/* shifted right-arrow key */
#define KEY_SRSUME		0623		/* shifted resume key */
#define KEY_SSAVE		0624		/* shifted save key */
#define KEY_SSUSPEND	0625		/* shifted suspend key */
#define KEY_SUNDO		0626		/* shifted undo key */
#define KEY_SUSPEND		0627		/* suspend key */
#define KEY_UNDO		0630		/* undo key */
#define KEY_MOUSE		0631		/* Mouse event has occurred */

# define	reg	register

#ifndef TRUE
# define	TRUE	(true)
# define	FALSE	(false)
#endif

# undef		ERR
# define	ERR	(-1)
# undef		OK
# define	OK	(0)

# define	_ENDLINE	1	/* set if window is full width */
# define	_FULLWIN	2	/* set if the window tlc is 0,0 */
# define	_SCROLLWIN	4	/* set if window is full height */
# define	_FULLLINE	8	/* if FULLWIN and terminal has add/delete line termcap entries */
# define	_IDLINE		16	/* use terminal insert/delete line. NO EFFECT */
# define	_NOCHANGE	-1

# ifdef DEBUG
extern FILE *curses_trace_file;
# endif

typedef	unsigned long	chtype;
typedef chtype			attr_t;

struct _win_st {
    short	_cury, _curx;	/* current cursor position */
    short	_maxy, _maxx;	/* n.b. this is a size, not a maximum index */
    short	_begy, _begx;	/* tlc of window. possibly non-zero for subwindows */
    short	_regtop;		/* scroll region */
    short	_regbottom;		/* scroll region */
    short	_flags;
    attr_t	_attrs;			/* current attribute for non-space chars */
    chtype	_bkgd;			/* current background char/attribute pair */

    /* for subwindows, this is the x offset from the start of the parent window
    also needs to be added to every entry in _firstch and _lastch  */
    short	_ch_off;		

    bool	_clear;			/* true if the next refresh should repaint the whole screen */
    bool	_leave;
    bool	_scroll;		/* true if we can scroll */
    bool	_use_keypad;
    int		_delay;
    bool	_no_delay;
    int		_color;			/* current pair for non-spacing chars */

    /* An array of pointers to arrays of chtype indexed by [y][x].
    Rows are individually malloc'd so the block will NOT be contiguous in RAM.
    For example, to iterate the whole screen you must iterate by [y] and then by [x] */
    chtype	**_y;
    short	*_firstch;		/* indexed by [y], gives the x + _ch_off of the first change in a line */
    short	*_lastch;		/* indexed by [y], gives the x + _ch_off of the last change in a line */
    struct _win_st	*_nextp, *_orig;	/* pointers to parent and subwindows if used */
};

typedef struct _win_st	WINDOW;

extern WINDOW   *stdscr, *curscr;
extern int      LINES, COLS, COLORS, COLOR_PAIRS, ESCDELAY, TABSIZE;
extern bool     _rawmode, _echoit, _pfast;
extern long     _ttyflags;

/*
 *	Define VOID to stop lint from generating "null effect"
 * comments.
 */
# ifdef lint
int	__void__;
# define	VOID(x)	(__void__ = (int) (x))
# else
# define	VOID(x)	(x)
# endif

/*
 * attribute and property macros
 */
#define	attroff(a)			wattroff(stdscr, a)
#define	attron(a)			wattron(stdscr, a)
#define	attrset(a)			wattrset(stdscr, a)
#define	bkgdset(ch)			wbkgdset(stdscr, ch)
#define	clearok(win, bf)	(int)(win == NULL ? OK : (win->_clear = bf, OK))	
#define	getbegx(win)		(int)(win == NULL ? ERR : win->_begx)
#define	getbegy(win)		(int)(win == NULL ? ERR : win->_begy)
#define	getbegyx(win, y, x)	(void)(y = getbegy(win), x = getbegx(win))
#define	getcurx(win)		(int)(win == NULL ? ERR : win->_curx)
#define	getcury(win)		(int)(win == NULL ? ERR : win->_cury)
#define	getmaxx(win)		(int)(win == NULL ? ERR : win->_maxx)
#define	getmaxy(win)		(int)(win == NULL ? ERR : win->_maxy)
#define	getmaxyx(win, y, x)	(void)(y = getmaxy(win), x = getmaxx(win))
#define	getyx(win, y, x)	(void)(y = getcury(win), x = getcurx(win))
#define	leaveok(win, bf)	(int)(win == NULL ? OK : (win->_leave = bf, OK))
#define	scrollok(win, bf)	(int)(win == NULL ? OK : (win->_scroll = bf, OK))
#define	standend()			attrset(A_NORMAL)
#define	standout()			attron(A_STANDOUT)
#define	wattroff(win, a)	(int)(win == NULL ? ERR : (win->_attrs &= ~a, OK))
#define	wattron(win, a)		(int)(win == NULL ? ERR : (win->_attrs |= a, OK))
#define	wattrset(win, a)	(int)(win == NULL ? ERR : (win->_attrs = a, OK))
#define idlok(win, bf)		(int)(win == NULL ? OK : (bf ? win->_flags |= _IDLINE : win->_flags &= ~_IDLINE, OK))
#define keypad(win, bf)		(int)(win == NULL ? ERR : (win->_use_keypad = bf, OK))
#define nodelay(win, bf)	(int)(win == NULL ? ERR : (win->_no_delay = bf, OK))
#define termattrs()			(chtype)(A_NORMAL|A_REVERSE|A_COLOR)
#define timeout(delay)		(void)(stdscr->_delay = delay)

/*
 * psuedo functions
 */
#define	addch(ch)	VOID(waddch(stdscr, ch))
#define	getch()		VOID(wgetch(stdscr))
#define	addstr(str)	VOID(waddbytes(stdscr, str, (int)strlen(str)))
#define	getstr(str)	VOID(wgetstr(stdscr, str))
#define	move(y, x)	VOID(wmove(stdscr, y, x))
#define	clear()		VOID(wclear(stdscr))
#define	erase()		VOID(werase(stdscr))
#define	clrtobot()	VOID(wclrtobot(stdscr))
#define	clrtoeol()	VOID(wclrtoeol(stdscr))
#define	insertln()	VOID(winsertln(stdscr))
#define	deleteln()	VOID(wdeleteln(stdscr))
#define	refresh()	VOID(wrefresh(stdscr))
#define	inch()		VOID(winch(stdscr))
#define	insch(c)	VOID(winsch(stdscr,c))
#define	delch()		VOID(wdelch(stdscr))
#define setscrreg(top, bottom)	wsetscrreg(stdscr, top, bottom)
#define scrl(n)					wscrl(stdscr, n)
#define scroll(win)				wscrl(win, 1)

/*
 * mv functions
 */
#define	mvwaddch(win,y,x,ch)	VOID(wmove(win,y,x)==ERR?ERR:waddch(win,ch))
#define	mvwgetch(win,y,x)	VOID(wmove(win,y,x)==ERR?ERR:wgetch(win))
#define	mvwaddstr(win,y,x,str) \
        VOID(wmove(win,y,x)==ERR?ERR:waddbytes(win,str,(int)strlen(str)))
#define mvwgetstr(win,y,x,str)  VOID(wmove(win,y,x)==ERR?ERR:wgetstr(win,str))
#define	mvwinch(win,y,x)	VOID(wmove(win,y,x) == ERR ? ERR : winch(win))
#define	mvwdelch(win,y,x)	VOID(wmove(win,y,x) == ERR ? ERR : wdelch(win))
#define	mvwinsch(win,y,x,c)	VOID(wmove(win,y,x) == ERR ? ERR:winsch(win,c))
#define	mvaddch(y,x,ch)		mvwaddch(stdscr,y,x,ch)
#define	mvgetch(y,x)		mvwgetch(stdscr,y,x)
#define	mvaddstr(y,x,str)	mvwaddstr(stdscr,y,x,str)
#define mvgetstr(y,x,str)   mvwgetstr(stdscr,y,x,str)
#define	mvinch(y,x)         mvwinch(stdscr,y,x)
#define	mvdelch(y,x)		mvwdelch(stdscr,y,x)
#define	mvinsch(y,x,c)		mvwinsch(stdscr,y,x,c)

/*
 * psuedo functions
 */

#define	winch(win)  (win->_y[win->_cury][win->_curx])
#define raw()       (_ttyflags |= RAW, _pfast = _rawmode = TRUE)
#define noraw()     (_ttyflags &= ~RAW, _rawmode = FALSE, _pfast = !(_ttyflags & CRMOD))
#define cbreak()    (_ttyflags |= CBREAK, _rawmode = TRUE)
#define nocbreak()  (_ttyflags &= ~CBREAK, _rawmode = FALSE)
#define crmode()    cbreak()	/* backwards compatability */
#define nocrmode()  nocbreak()	/* backwards compatability */
#define echo()      (_ttyflags |= ECHO, _echoit = TRUE)
#define noecho()    (_ttyflags &= ~ECHO, _echoit = FALSE)
#define nl()        (_ttyflags |= CRMOD, _pfast = _rawmode)
#define nonl()      (_ttyflags &= ~CRMOD, _pfast = TRUE)
#define	erasechar()	(ERR)
#define	killchar()	(ERR)
#define baudrate()	(19200)
#define longname()  ("VT52 ATARI")

__EXTERN int		waddbytes __PROTO((WINDOW *, const char *, int));
__EXTERN int		waddchtypes __PROTO((WINDOW *, const chtype *, int));
__EXTERN int		box __PROTO((WINDOW *, chtype, chtype));
__EXTERN int		delwin __PROTO((WINDOW *));
__EXTERN int		endwin __PROTO((void));
__EXTERN char *		fullname __PROTO((char *, char *));
__EXTERN char *		getcap __PROTO((char *));
__EXTERN void		gettmode __PROTO((void));
__EXTERN WINDOW *	initscr __PROTO((void));
__EXTERN int		mvcur __PROTO((int, int, int, int));
__EXTERN int		mvprintw __PROTO((int, int, char *, ...));
__EXTERN int 		mvwprintw __PROTO((WINDOW *, int, int, char *, ...));
__EXTERN int		mvwscanw __PROTO((WINDOW *, int, int, char *, ...));
__EXTERN int		mvwin __PROTO((WINDOW *, int, int));
__EXTERN WINDOW *	newwin __PROTO((int, int, int, int));
__EXTERN void		overlay __PROTO((WINDOW *, WINDOW *));
__EXTERN int		overwrite __PROTO((WINDOW *, WINDOW *));
__EXTERN int		printw __PROTO((char *, ...));
__EXTERN int		setterm __PROTO((char *));
__EXTERN WINDOW *	subwin __PROTO((WINDOW *, int, int, int, int));
__EXTERN int		tabcol __PROTO((int, int));
__EXTERN int		touchline __PROTO((WINDOW *, int, int));
__EXTERN void		touchoverlap __PROTO((WINDOW *, WINDOW *));
__EXTERN int		touchwin __PROTO((WINDOW *));
__EXTERN int		waddch __PROTO((WINDOW *, const chtype));
__EXTERN int		waddstr __PROTO((WINDOW *, char *));
__EXTERN int		wclear __PROTO((WINDOW *));
__EXTERN int		wclrtobot __PROTO((WINDOW *));
__EXTERN int		wclrtoeol __PROTO((WINDOW *));
__EXTERN int		wdelch __PROTO((WINDOW *));
__EXTERN int		wdeleteln __PROTO((WINDOW *));
__EXTERN int		werase __PROTO((WINDOW *));
__EXTERN int		wgetch __PROTO((WINDOW *));
__EXTERN int		wgetstr __PROTO((WINDOW *, char *));
__EXTERN int		winsch __PROTO((WINDOW *, chtype));
__EXTERN int		winsertln __PROTO((WINDOW *));
__EXTERN int		wmove __PROTO((WINDOW *, int, int));
__EXTERN int		wprintw __PROTO((WINDOW *, char *, ...));
__EXTERN int		wrefresh __PROTO((WINDOW *));
__EXTERN int		wscanw __PROTO((WINDOW *, char *, ...));
__EXTERN void		zap __PROTO((void));
__EXTERN char		*unctrl __PROTO((int c));

__EXTERN bool		has_colors __PROTO((void));
__EXTERN int		start_color __PROTO((void));
__EXTERN int		beep __PROTO((void));
__EXTERN int		mvaddnstr __PROTO((int y, int x, const char *str, int n));
__EXTERN int		curs_set __PROTO((int visibility));
__EXTERN int		pair_content __PROTO((short pair, short *f, short *b));
__EXTERN int		wscrl __PROTO((WINDOW *, int n));
__EXTERN int		init_pair __PROTO((short pair, short f, short b));
__EXTERN int		wsetscrreg __PROTO((WINDOW *, int top, int bot));
__EXTERN void		wbkgdset __PROTO((WINDOW *, chtype ch));
__EXTERN int		scr_dump __PROTO((const char *filename));

#define scanw(fmt, ...)         wscanw(stdscr, fmt, ...)
#define mvscanw(y, x, fmt, ...) mvwscanw(stdscr, y, x, fmt, ...)

/* not implemented */
__EXTERN int		intrflush __PROTO((WINDOW *win, bool bf));
__EXTERN WINDOW *	newpad __PROTO((int nlines, int ncols));
__EXTERN int		copywin __PROTO((const WINDOW *srcwin, WINDOW *dstwin,
                        int sminrow, int smincol, int dminrow, int dmincol,
                        int dmaxrow, int dmaxcol, int overlay));
/* some useful non-curses functionality */
__EXTERN bool		waitchar __PROTO((long ms_wait));

#endif
