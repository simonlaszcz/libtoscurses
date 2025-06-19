#include "internal.h"
#include <stdlib.h>
#include <string.h>
#include <osbind.h>
#include <mint/sysvars.h>
#include <time.h>

#define CODE(kb,sc)     (long)(((long)kb<<24)|((long)sc<<16))

#define KB_RSHIFT       (1)
#define KB_LSHIFT       (2)
#define KB_CTRL         (4)
#define KB_ALT          (8)
#define KB_CAPS         (16)
#define KB_RMOUSE       (32)
#define KB_LMOUSE       (64)
#define KB_ALTGR        (128)

#define SC_F1           CODE(0, 59)
#define SC_F2           CODE(0, 61)
#define SC_F3           CODE(0, 62)
#define SC_F4           CODE(0, 63)
#define SC_F5           CODE(0, 64)
#define SC_F6           CODE(0, 65)
#define SC_F7           CODE(0, 66)
#define SC_F8           CODE(0, 67)
#define SC_F9           CODE(0, 68)
#define SC_F10          CODE(0, 69)
#define SC_S_F1         CODE(0, 84)
#define SC_S_F2         CODE(0, 85)
#define SC_S_F3         CODE(0, 86)
#define SC_S_F4         CODE(0, 87)
#define SC_S_F5         CODE(0, 88)
#define SC_S_F6         CODE(0, 89)
#define SC_S_F7         CODE(0, 90)
#define SC_S_F8         CODE(0, 91)
#define SC_S_F9         CODE(0, 92)
#define SC_S_F10        CODE(0, 93)
#define SC_UP           CODE(0, 72)
#define SC_DOWN         CODE(0, 80)
#define SC_LEFT         CODE(0, 75)
#define SC_RIGHT        CODE(0, 77)
#define SC_UNDO         CODE(0, 97)
#define SC_INSERT       CODE(0, 82)
#define SC_HOME         CODE(0, 71)
#define SC_BACKSPACE    CODE(0, 14)
#define SC_ENTER        CODE(0, 114)
#define SC_DEL          CODE(0, 83)
#define SC_S_UP         CODE(KB_LSHIFT, 72)
#define SC_S_DOWN       CODE(KB_LSHIFT, 80)
#define SC_S_LEFT       CODE(KB_LSHIFT, 75)
#define SC_S_RIGHT      CODE(KB_LSHIFT, 77)

static struct key_mapping
{
    long code;
    int curses_code;
}
/* the codes returned by Crawcin() just have a scancode, i.e. no ascii code */
key_mappings[] =
{
    {SC_F1,			KEY_F(1)},
    {SC_F2,			KEY_F(2)},
    {SC_F3,			KEY_F(3)},
    {SC_F4,			KEY_F(4)},
    {SC_F5,			KEY_F(5)},
    {SC_F6,			KEY_F(6)},
    {SC_F7,			KEY_F(7)},
    {SC_F8,			KEY_F(8)},
    {SC_F9,			KEY_F(9)},
    {SC_F10,		KEY_F(10)},
    {SC_S_F1,		KEY_F(11)},
    {SC_S_F2,		KEY_F(12)},
    {SC_S_F3,		KEY_F(13)},
    {SC_S_F4,		KEY_F(14)},
    {SC_S_F5,		KEY_F(15)},	
    {SC_S_F6,		KEY_F(16)},
    {SC_S_F7,		KEY_F(17)},
    {SC_S_F8,		KEY_F(18)},
    {SC_S_F9,		KEY_F(19)},
    {SC_S_F10,		KEY_F(20)},
    {SC_UP,			KEY_UP},
    {SC_DOWN,		KEY_DOWN},
    {SC_LEFT,		KEY_LEFT},
    {SC_RIGHT,		KEY_RIGHT},
    {SC_UNDO,		KEY_UNDO},
    {SC_INSERT,		KEY_IC},
    {SC_HOME,		KEY_HOME},
    {SC_BACKSPACE,	KEY_BACKSPACE},
    {SC_ENTER,		KEY_ENTER},
    {SC_DEL,		KEY_DC},
    {SC_S_UP,		KEY_PPAGE},
    {SC_S_DOWN,		KEY_NPAGE},
    {SC_S_RIGHT,	KEY_END},
    {0,	0}
};

#define GOT_CHAR		(-1)

static struct
{
    char old_conterm;
} g;

static struct key_mapping *lookup(long cin);
static void sort_keymapping_table(struct key_mapping *t);
static int sort_keymapping_desc(const void *va, const void *vb);
static int super_init(void);
static int super_restore(void);
static void tick(void);

/**
 * called from initscr() to set things up
 */
void
init_getch(void)
{
    memset(&g, 0, sizeof(g));
    sort_keymapping_table(key_mappings);
    Supexec(super_init);
    /* disable mouse and joystick events */
    char kbs[] = {0x12, 0x1a};
    Ikbdws(1, kbs);
    curs_set(1);
}

static int
super_init(void)
{
    char v = g.old_conterm = *conterm;

    /* no key click */
    v &= 0xFE;
    /* kbshift keys on */
    v |= 0x08;
    *conterm = v;

    return 0;
}

void
restore_getch(void)
{
    Supexec(super_restore);
    /* enable mouse & joystick reporting */
    char kbs[] = {0x15, 0x08};
    Ikbdws(1, kbs);
}

static int
super_restore(void)
{
    *conterm = g.old_conterm;
    return 0;
}

/**
 * wait for a character to be available on stdin.
 * wait upto ms_wait or indefitely if ms_wait is -1
 * return true if a character is available, otherwise false
 */
bool
waitchar(long ms_wait)
{
    tracev3("ms_wait=%ld", ms_wait);

    if (Cconis() == GOT_CHAR)
        return true;

    if (ms_wait == 0)
        return false;

    if (ms_wait > 0) {
        long cycles = ms_wait / 5;

        do {
            if (Cconis() == GOT_CHAR)
                return true;
            tick();
        } while (--cycles > 0);

        return false;
    }

    while (true) {
        tick();
        if (Cconis() == GOT_CHAR)
            return true;
    }

    return false;
}

static void
tick(void)
{
    clock_t start = clock();
    do {} while (clock() == start);
}

//TODO: handle cbreak
/**
 * This routine reads in a character from the window.
 */
int
wgetch(WINDOW *win)
{
    tracev3("win=%p, echo=%d", win, _echoit);

    if (!waitchar(win->_no_delay ? 0 : win->_delay))
        return ERR;

    long raw = Crawcin();
    long scan = raw & 0xFFFF0000;
    int asc = raw & 0xFF;
    int val = asc;
    tracev3("win=%p, scancode=%d, asc=%c", win, (int)(scan >> 16), asc);

    if (asc > 0) {
        switch (asc) {
        case 3:     /* ^C */
        case 17:    /* ^Q */
        case 26:    /* *Z */
            exit(endwin() == OK ? EXIT_SUCCESS : EXIT_FAILURE);
        }
    }

    if (scan > 0 && win->_use_keypad) {
        struct key_mapping *e = lookup(scan);
        if (e != NULL) {
            val = e->curses_code;
            asc = 0;
            tracev3("win=%p, curses_code=%d", win, val);
        }
    }

    if (_echoit && asc > 0) 
        waddch(win, asc);

    return val;
}

static struct key_mapping *
lookup(long cin)
{
    for (int i = 0; true; ++i) {
        if (key_mappings[i].code == 0 || cin > key_mappings[i].code)
            break;
        if (key_mappings[i].code == cin)
            return &key_mappings[i];
    }

    return NULL;
}

static void
sort_keymapping_table(struct key_mapping *t)
{
    int len = 0;
    for (int i = 0; t[i].code != 0; ++i)
        ++len;
    qsort(t, len, sizeof(struct key_mapping), sort_keymapping_desc);
}

static int
sort_keymapping_desc(const void *va, const void *vb)
{
    struct key_mapping *a = (struct key_mapping *)va;
    struct key_mapping *b = (struct key_mapping *)vb;
    return b->code - a->code;
}
