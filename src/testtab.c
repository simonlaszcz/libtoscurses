#include <curses.h>

main()
{
    initscr();
    clear();
    move(9,0);
    addstr("123456781234567812345678123456781234567812345678");

    move(10,0);
    addch('\t'); addch('8'); addch('\t'); addch('8'); addch('\t'); addch('8');
    addch('\t'); addch('8');

    move(15,0);
    addstr("\t8\t8\t8\t8");

    move(17,0); addstr("hit return to finish -->");
    refresh();
    getch();
    endwin();
}

    