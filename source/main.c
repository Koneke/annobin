#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define _BSD_SOURCE
#define __USE_BSD
#include <endian.h>
#include <unistd.h>

#include <ncurses.h>

#include "draw.h"
#include "global.h"
#include "comment.h"
#include "common.h"
#include "file.h"
#include "input.h"
#include "cursor.h"

int main(int argc, char** argv)
{
	initscr();
	start_color();
	curs_set(0);
	init_pair(1, COLOR_BLACK, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_RED); // cursor
	init_pair(4, COLOR_RED, COLOR_WHITE); // commenting area
	init_pair(5, COLOR_WHITE, COLOR_RED); // important
	init_pair(6, COLOR_WHITE, COLOR_BLUE);
	init_pair(7, COLOR_WHITE, COLOR_GREEN);
	noecho();

	file = fopen("in.testfile", "r");
	readannotfile("annot.testfile");

	getmaxyx(stdscr, h, w);

	bytesperline = 16;
	bytescroll = 0;

	int size = 20000;
	buffer = malloc(size);
	fread(buffer, 1, size, file);

	run = 1;
	state = 0; // 0 browse, 1 insert, 2 write
	while (run)
	{
		draw();
		input();
	}

	freeallcomments();

	free(buffer);
	endwin();

	return 0;
}
