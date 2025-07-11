#include "internal.h"
#include <string.h>
#include <stdlib.h>
#include <osbind.h>
#include <xyzst/xyz.h>

#define UNSET   (-1)
#define NCOLORS (8)

static struct
{
    bool was_started;
    /* true if we can change the palette */
    bool can_modify_palette;
    /* true if the cursor has a separate palette entry */
    bool can_modify_cursor;
    /* valid when can_modify_cursor */
    short cursor_palidx;
    short num_colors;
    /* the last index in the palette, by default the fg */
    short fg_palidx;
    /* indexed by curses color, gives the corresponding tos color */
    short color_map[NCOLORS];
    /* dominant curses background color */
    short bg_color;
    xyz_palette_t saved_palette;
    struct color_pair
    {
        short fg;
        short bg;
    } *color_pairs;
} m;

static void swap_palbg(int c);

void
init_color(void)
{
    memset(&m, 0, sizeof(m));
    m.num_colors = 2;
    m.fg_palidx = 1;

    struct xyz_con_info con;
    struct xyz_os_info os;

    if (xyz_get_con_info(&con) == XYZ_OK && xyz_get_os_info(&os) == XYZ_OK) {
        m.num_colors = con.num_colors;
        m.fg_palidx = con.num_colors - 1;
#ifndef TOSCOMPAT
        if (con.is_palette_used && os.type == XYZ_OS_TOS) {
            m.can_modify_palette = true;
            m.cursor_palidx = con.cursor_palidx;
            m.can_modify_cursor = con.num_colors > NCOLORS && con.cursor_palidx >= NCOLORS;
        }
#endif
    }

    if (m.can_modify_palette) {
        m.saved_palette = xyz_conpal_save();
        /* white...black */
        xyz_conpal_standard();
        /* swap black and white */
        swap_palbg(m.fg_palidx);
        xyz_palette_eset(m.cursor_palidx, xyz_white());
        m.bg_color = COLOR_BLACK;
    }
    else {
        m.bg_color = COLOR_WHITE;
    }

    tracev1("num_colors=%d, can_modify_palette=%d", m.num_colors, m.can_modify_palette);
}

bool
has_colors(void)
{
    return m.num_colors > 2;
}

int
start_color(void)
{
    if (m.was_started)
        return OK;

    trace1();

    /* if start_color() is called on a mono system then has_colors() has not been
       called or was ignored. we do our best by repeating colors. let's just claim we support
       all standard curses colors */
    COLORS = NCOLORS;
    COLOR_PAIRS = NCOLORS * NCOLORS;

    int sz = sizeof(struct color_pair) * COLOR_PAIRS;
    if ((m.color_pairs = malloc(sz)) == NULL)
        goto ret_err;
    memset(m.color_pairs, 0, sz);
    for (int i = 0; i < COLOR_PAIRS; ++i)
        m.color_pairs[i].fg = UNSET;

    if (m.can_modify_palette) {
        if (m.can_modify_cursor) {
            /* only use NCOLORS of the palette. on a 16 color system this will
               allow the cursor to have its own entry */
            m.fg_palidx = NCOLORS - 1;
            xyz_palette_eset(m.fg_palidx, xyz_white());
            xyz_palette_eset(m.cursor_palidx, xyz_white());
        }

        m.color_pairs[0].bg = COLOR_BLACK;
        m.color_pairs[0].fg = COLOR_WHITE;
        m.color_map[COLOR_BLACK] = 0;
        m.color_map[COLOR_WHITE] = m.fg_palidx;
    }
    else {
        m.color_pairs[0].bg = COLOR_WHITE;
        m.color_pairs[0].fg = COLOR_BLACK;
        m.color_map[COLOR_BLACK] = m.fg_palidx;
        m.color_map[COLOR_WHITE] = 0;
    }

    /* only one curses color can be mapped to pal0 */
    switch (m.num_colors) {
    case 2:
        for (int i = COLOR_RED; i < COLOR_WHITE; i++)
            m.color_map[i] = 1;
        break;
    case 4:
        for (int i = COLOR_RED; i < COLOR_WHITE; i += 2) {
            m.color_map[i + 0] = 1;
            m.color_map[i + 1] = 2;
        }
        break;
    default:
        for (int i = COLOR_RED; i < COLOR_WHITE; i++)
            m.color_map[i] = i;
        break;
    }

    m.was_started = true;

    return OK;
ret_err:
    free(m.color_pairs);
    return ERR;
}

void
fix_cursor_color(void)
{
    if (!(m.was_started && m.can_modify_cursor))
        return;

    chtype ch = stdscr->_y[stdscr->_cury][stdscr->_curx];
    short f, b = m.bg_color;

    if (ch & A_COLOR) {
        if (pair_content(PAIR_NUMBER(ch), &f, &b) == ERR)
            b = m.bg_color;
        else if (ch & A_REVERSE)
            b = f;
    }

    /* get an appropriate palette index number for the cursor */

    switch (b) {
    case COLOR_WHITE:
    case COLOR_YELLOW:
        xyz_palette_eset(m.cursor_palidx, xyz_black());
        break;
    default:
        xyz_palette_eset(m.cursor_palidx, xyz_white());
        break;
    }
}

/*
 * swap tos color c with palette entry 0
 */
static void
swap_palbg(int c)
{
    tracev1("c=%d", c);

    if (m.can_modify_palette && c != 0)
        xyz_palette_eswap(0, c);
}

void
sync_bg(WINDOW *win)
{
    if (!(m.was_started && win != NULL && m.can_modify_palette))
        return;

    /* match the perimeter color of the curses screen with pal0 */

    short stats[NCOLORS] = { 0 };
    short dominant = m.bg_color;
    short half = win->_maxx + win->_maxy - 2;
    short lasty = win->_maxy - 1;
    short lastx = win->_maxx - 1;

    /* horizontals */
    for (short x = 0; x < win->_maxx && stats[dominant] < half; ++x) {
        short f, b;

        chtype *ch = &(win->_y[0][x]);
        if (pair_content(PAIR_NUMBER(*ch), &f, &b) == OK) {
            if (*ch & A_REVERSE)
                b = f;
            ++stats[b];
            if (b != dominant && stats[b] > stats[dominant])
                dominant = b;
        }

        ch = &(win->_y[lasty][x]);
        if (pair_content(PAIR_NUMBER(*ch), &f, &b) == OK) {
            if (*ch & A_REVERSE)
                b = f;
            ++stats[b];
            if (b != dominant && stats[b] > stats[dominant])
                dominant = b;
        }
    }

    /* verticals */
    for (short y = 1; y < lasty && stats[dominant] < half; ++y) {
        short f, b;

        chtype *ch = &(win->_y[y][0]);
        if (pair_content(PAIR_NUMBER(*ch), &f, &b) == OK) {
            if (*ch & A_REVERSE)
                b = f;
            ++stats[b];
            if (b != dominant && stats[b] > stats[dominant])
                dominant = b;
        }

        ch = &(win->_y[y][lastx]);
        if (pair_content(PAIR_NUMBER(*ch), &f, &b) == OK) {
            if (*ch & A_REVERSE)
                b = f;
            ++stats[b];
            if (b != dominant && stats[b] > stats[dominant])
                dominant = b;
        }
    }

    /* if the dominant color is already palette index 0, then no change */
    if (m.color_map[dominant] == 0)
        return;

    /* else find current 0 and swap */
    int i = 0;
    while (i < NCOLORS && m.color_map[i] != 0)
        ++i;

    if (i == NCOLORS || i == dominant)
        return;

    tracev1("dominant=%d", dominant);
    swap_palbg(m.color_map[dominant]);
    short tmp = m.color_map[i];
    m.color_map[i] = m.color_map[dominant];
    m.color_map[dominant] = tmp;
    m.bg_color = dominant;
}

short
screen_bg_color(void)
{
    return m.was_started ? m.bg_color :
        m.can_modify_palette ? COLOR_BLACK : COLOR_WHITE;
}

short
screen_bg_tos_color(void)
{
    return m.was_started ? m.color_map[m.bg_color] : 0;
}

void
restore_color(void)
{
    /* n.b. we may save the palette before start_color() */
    if (m.saved_palette != NULL) {
        xyz_palette_set(m.saved_palette);
        free(m.saved_palette);
    }

    if (!m.was_started)
        return;

    m.was_started = false;
    free(m.color_pairs);
}

int
init_pair(short pair, short f, short b)
{
    tracev1("pair=%d, f=%d, b=%d", pair, f, b);

    /* default pair 0 cannot be amended */
    if (!(m.was_started && pair > 0 && pair < COLOR_PAIRS && f >= 0 && f < COLORS && b >= 0 && b < COLORS))
        return ERR;

    struct color_pair *p = &(m.color_pairs[pair]);

    /* if a pair is redefined we need to repaint the whole screen */
    if (p->fg != UNSET)
        clearok(curscr, true);

    p->fg = f;
    p->bg = b;

    return OK;
}

int
pair_content(short pair, short *f, short *b)
{
    if (!(m.was_started && pair >= 0 && pair < COLOR_PAIRS))
        return ERR;

    struct color_pair *p = &(m.color_pairs[pair]);

    if (p->fg != UNSET) {
        *f = p->fg;
        *b = p->bg;
        return OK;
    }

    return ERR;
}

int
tos_pair_content(short pair, short *f, short *b)
{
    if (!(m.was_started && pair >= 0 && pair < COLOR_PAIRS))
        return ERR;

    struct color_pair *p = &(m.color_pairs[pair]);

    if (p->fg != UNSET) {
        *f = m.color_map[p->fg];
        *b = m.color_map[p->bg];
        return OK;
    }

    return ERR;
}
