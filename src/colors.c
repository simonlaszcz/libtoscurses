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
    short num_colors;
    struct color_pair
    {
        short fg;
        short bg;
    } *color_pairs;
    /* indexed by curses color, gives the corresponding tos color */
    short color_map[NCOLORS];
    /* dominant curses background color */
    short bg_color;
#ifndef TOSCOMPAT
    xyz_palette_t saved_palette;
#endif
} m;

#ifndef TOSCOMPAT
static void swap_palbg(int c);
#endif

void
init_color(void)
{
    memset(&m, 0, sizeof(m));

    struct xyz_con_info con;
    if (xyz_get_con_info(&con) == XYZ_OK)
        m.num_colors = con.num_colors;
    else
        m.num_colors = 2;

#ifndef TOSCOMPAT
    m.saved_palette = xyz_palbank0_save();
    xyz_palbank0_standard();
    Vsync();
    /* swap black and white */
    swap_palbg(m.num_colors - 1);
    m.bg_color = COLOR_BLACK;
#else
    m.bg_color = COLOR_WHITE;
#endif

    tracev1("num_colors=%d", m.num_colors);
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

#ifndef TOSCOMPAT
    m.color_pairs[0].bg = COLOR_BLACK;
    m.color_pairs[0].fg = COLOR_WHITE;
    m.color_map[COLOR_BLACK] = 0;
    m.color_map[COLOR_WHITE] = m.num_colors - 1;
#else
    m.color_pairs[0].bg = COLOR_WHITE;
    m.color_pairs[0].fg = COLOR_BLACK;
    m.color_map[COLOR_BLACK] = m.num_colors - 1;
    m.color_map[COLOR_WHITE] = 0;
#endif

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

/*
 * swap tos color c with palette entry 0
 */
#ifndef TOSCOMPAT
static void
swap_palbg(int c)
{
    tracev1("c=%d", c);

    if (c != 0)
        xyz_palbank0_swap(0, c);
}
#endif

#ifndef TOSCOMPAT
void
sync_bg(WINDOW *win)
{
    if (!m.was_started)
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
#endif

short
screen_bg_color(void)
{
#ifndef TOSCOMPAT
    return m.was_started ? m.bg_color : COLOR_BLACK;
#else
    return m.was_started ? m.bg_color : COLOR_WHITE;
#endif
}

short
screen_bg_tos_color(void)
{
    return m.was_started ? m.color_map[m.bg_color] : 0;
}

void
restore_color(void)
{
#ifndef TOSCOMPAT
    /* n.b. we may save the palette before start_color() */
    if (m.saved_palette != NULL) {
        xyz_palbank0_set(m.saved_palette);
        free(m.saved_palette);
    }
#endif

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
    if (!
        (m.was_started && pair > 0 && pair < COLOR_PAIRS && f >= 0 && f < COLORS && b >= 0
         && b < COLORS))
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
