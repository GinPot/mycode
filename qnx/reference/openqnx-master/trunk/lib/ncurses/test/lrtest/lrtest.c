/*
 * Test lower-right-hand corner access
 *
 * by Eric S. Raymond <esr@thyrsus.com>
 *
 * This can't be part of the ncurses test-program, because ncurses rips off the
 * bottom line to do labels.
 *
 * $Id: lrtest.c 153052 2008-08-13 01:17:50Z coreos $
 */

#include <test.priv.h>

int
main(
	int argc GCC_UNUSED,
	char *argv[] GCC_UNUSED)
{
    initscr();

    move(LINES/2-1, 4);
    if (!has_ic())
    {
	addstr("Your terminal lacks the capabilities needed to address the\n");
	move(LINES/2, 4);
	addstr("lower-right-hand corner of the screen.\n");
    }
    else
    {
	addstr("This is a test of access to the lower right corner.\n");
	move(LINES/2, 4);
	addstr("If the top of the box is missing, the test failed.\n");
	move(LINES/2+1, 4);
	addstr("Please report this (with a copy of your terminfo entry).\n");
	move(LINES/2+2, 4);
	addstr("to the ncurses maintainers, at bug-ncurses@gnu.org.\n");
    }

    box(stdscr, 0, 0);
    move(LINES-1, COLS-1);

    refresh();

    getch();
    endwin();
    return 0;
}

/* lrtest.c ends here */
