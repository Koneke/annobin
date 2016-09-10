#include <stdint.h>

#include <ncurses.h>

#include "common.h"
#include "message.h"
#include "draw.h"

static int leftmarginWidth;
static char leftmarginFormat[20];
static char leftmarginBlankFormat[20];

static char getprintchar(char c)
{
	if (
		(c >= 'A' && c <= 'Z') || 
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9')
	) {
		return c;
	}

	return '_';

	switch(c)
	{
		case 0:
		case 0x0d: // cr
			return ' ';
	}

	return c;
}

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

	view_bytesperline = 16;
	view_bytescroll = 0;
	view_byteOffset = 0;
}

void draw_postSetup()
{
	int temp = file_size;
	while (temp)
	{
		temp /= 10;
		leftmarginWidth++;
	}

	sprintf(leftmarginFormat, "%%0%ix", leftmarginWidth);
	sprintf(leftmarginBlankFormat, "%%-%is", leftmarginWidth);
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

	if (
		model_selectionstart != -1 &&
		offset >= model_selection_firstOffset() &&
		offset <= model_selection_lastOffset())
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
	int eof = 0;

	for (int y = 0; (!eof) && y < view_height - 1; y++)
	{
		offset = model_bufferoffset + view_bytescroll + view_byteOffset + y * view_bytesperline;

		if (offset >= file_size)
		{
			eof = 1;
			break;
		}

		attron(COLOR_PAIR(1));
		if (offset < 0)
		{
			mvwprintw(stdscr, y, 0, leftmarginBlankFormat, " ");
		}
		else
		{
			mvwprintw(stdscr, y, 0, leftmarginFormat, offset);
		}
		attroff(COLOR_PAIR(1));

		for (int i = 0; i < view_bytesperline; i++)
		{
			// notice! not using model_bufferoffset!
			// INTENTIONAL
			offset = view_bytescroll + view_byteOffset + y * view_bytesperline + i;
			if (offset < 0) continue;
			else if (offset + model_bufferoffset >= file_size)
			{
				eof = 1;
				break;
			}

			setcolor(model_bufferoffset + offset);
			mvwprintw(stdscr, y, leftmarginWidth + 2 + i * 3, "%02x ", model_buffer[offset]);

			int charTableX = leftmarginWidth + 4 + i + 3 * view_bytesperline;

			if (model_displayMode == 0)
			{
				mvwprintw(stdscr, y, charTableX, "%c", getprintchar(model_buffer[offset]));
			}
			else if (model_displayMode == 1)
			{
				mvwprintw(stdscr, y, charTableX, "%c", getprintchar(model_translationTable[model_buffer[offset]]));
			}

			attroff(COLOR_PAIR(1));
			attroff(COLOR_PAIR(2));
		}
	}
}

static void drawcomments()
{
	int offset;
	int commentlast = -1; // line (so we don't overlap comments)
	int lastcommentindex = -1;

	for (int y = 0; y < view_height; y++)
	{
		for (int x = 0; x < view_bytesperline; x++)
		{
			// comment_at is buffer position independant!
			offset = model_bufferoffset + view_bytescroll + view_byteOffset + y * view_bytesperline + x;
			if (offset < 0) continue;

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

				if (comment->index > lastcommentindex)
				{
					int commenty = max(commentlast, y);
					int commentx = leftmarginWidth + 6 + 3 * view_bytesperline + view_bytesperline;

					mvwprintw(stdscr, commenty, commentx, "%s", comment->comment);
					lastcommentindex = comment->index;
					commentlast = commenty + 1;
				}
			}
		}
	}
}

static void drawStatusLine()
{
	char format[10];
	char statusLine[view_width];
	sprintf(format, "%%-%is", view_width);
	sprintf(
		statusLine,
		"Table mode %i | Bytes per line %i | Offset %i",
		model_displayMode,
		view_bytesperline,
		view_byteOffset);

	attron(COLOR_PAIR(7));
	mvwprintw(stdscr, view_height - 1, 0, format, statusLine);
	attroff(COLOR_PAIR(7));
}

void draw_draw()
{
	clear();
	drawdata();
	drawcomments();
	drawStatusLine();
	refresh();
}
