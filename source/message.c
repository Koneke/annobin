#include <stdarg.h>
#include <curses.h>

void message_important(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	wbkgd(stdscr, COLOR_PAIR(2));
	attron(COLOR_PAIR(5));
	wmove(stdscr, 0, 0);
	vwprintw(stdscr, fmt, args);
	attroff(COLOR_PAIR(5));
	while ('y' != getch()) ;
	wbkgd(stdscr, COLOR_PAIR(0));
}
