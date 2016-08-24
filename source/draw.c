#include <stdint.h>

#include <ncurses.h>

#include "common.h"
#include "global.h"
#include "comment.h"

void drawcomments()
{
	int offset;
	int commentlast = -1; // line (so we don't overlap comments)
	int commentswritten = -1;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < bytesperline; x++)
		{
			offset = bytescroll + y * bytesperline + x;

			comment_t* comment;
			if (comment = commentat(offset))
			{
				if (comment == commentat(offsetfromxy(cursx, cursy)))
					attron(COLOR_PAIR(6 + (comment->index % 2)));
				else
					attron(COLOR_PAIR(1 + (comment->index % 2)));

				if (comment->index > commentswritten)
				{
					int commenty = max(commentlast, y);

					mvwprintw(stdscr, commenty, 28 + 3 * bytesperline, "%s", comment->comment);
					commentswritten++;
					commentlast = commenty + 1;
				}
			}
		}
	}
}

void draw()
{
	int offset;
	int commentlast = -1; // line (so we don't overlap comments)
	int commentswritten = -1;
	clear();
	for (int y = 0; y < h; y++)
	{
		offset = bytescroll + y * bytesperline;

		// print offset in left margin
		attron(COLOR_PAIR(1));
		mvwprintw(stdscr, y, 0, "%06x", offset);
		attroff(COLOR_PAIR(1));

		for (int i = 0; i < bytesperline; i++)
		{
			offset = bytescroll + y * bytesperline + i;

			comment_t* comment;
			if (comment = commentat(offset))
			{
				if (comment == commentat(offsetfromxy(cursx, cursy)))
					attron(COLOR_PAIR(6 + (comment->index % 2)));
				else
					attron(COLOR_PAIR(1 + (comment->index % 2)));
			}

			int comstart = min(commentstart, offsetfromxy(cursx, cursy));
			int comend = max(commentstart, offsetfromxy(cursx, cursy));

			if (state > 0 && offset >= comstart && offset < comend)
			{
				attron(COLOR_PAIR(4));
			}

			if (i == cursx && y == (cursy - screenscroll))
			{
				attron(COLOR_PAIR(3));
			}

			mvwprintw(stdscr, y, 8 + i * 3, "%02x ", buffer[offset]);

			move(y, 10 + 3 * bytesperline + i);
			printw("%c", getprintchar(buffer[offset]));

			attroff(COLOR_PAIR(1));
			attroff(COLOR_PAIR(2));
		}
		move(y, 0);
	}

	drawcomments();

	if (state == 2)
	{
		attron(COLOR_PAIR(3));
		mvwprintw(stdscr, 0, 0, "comment: %s", commentbuffer);
		attroff(COLOR_PAIR(3));
	}

	refresh();
}
