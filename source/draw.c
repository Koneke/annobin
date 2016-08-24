#include <stdint.h>

#include <ncurses.h>

#include "common.h"
#include "comment.h"
#include "model.h"
#include "view.h"

static const int leftmarginwidth = 6;

void draw_setup()
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
	getmaxyx(stdscr, view_height, view_width);
}

void draw_quit()
{
	endwin();
}

static void setcolor(int offset)
{
	comment_t* comment;
	if (comment = comment_at(offset))
	{
		if (comment == comment_highlighted)
		{
			attron(COLOR_PAIR(6 + (comment->index % 2)));
		}
		else
		{
			attron(COLOR_PAIR(1 + (comment->index % 2)));
		}
	}

	if (model_selectionstart != -1 && offset >= model_selectionstart && offset <= model_selectionend)
	{
		attron(COLOR_PAIR(4));
	}

	if (offset == model_cursoroffset)
	{
		attron(COLOR_PAIR(3));
	}
}

static void drawdata()
{
	int offset;
	int commentlast = -1; // line (so we don't overlap comments)
	int commentswritten = -1;
	for (int y = 0; y < view_height; y++)
	{
		offset = bytescroll + y * bytesperline;

		// print offset in left margin
		attron(COLOR_PAIR(1));
		mvwprintw(stdscr, y, 0, "%06x", offset);
		attroff(COLOR_PAIR(1));

		for (int i = 0; i < bytesperline; i++)
		{
			offset = bytescroll + y * bytesperline + i;

			setcolor(offset);

			mvwprintw(stdscr, y, 8 + i * 3, "%02x ", buffer[offset]);

			mvwprintw(stdscr, y, 10 + i + 3 * bytesperline,"%c", getprintchar(buffer[offset]));

			attroff(COLOR_PAIR(1));
			attroff(COLOR_PAIR(2));
		}
	}
}

static void drawcomments()
{
	int offset;
	int commentlast = -1; // line (so we don't overlap comments)
	int commentswritten = -1;

	for (int y = 0; y < view_height; y++)
	{
		for (int x = 0; x < bytesperline; x++)
		{
			offset = bytescroll + y * bytesperline + x;

			comment_t* comment;
			if (comment = comment_at(offset))
			{
				if (comment == comment_highlighted)
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

void draw_draw()
{
	clear();
	drawdata();
	drawcomments();
	refresh();
}
