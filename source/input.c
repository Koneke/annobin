#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "global.h"
#include "common.h"
#include "comment.h"
#include "cursor.h"
#include "file.h"
#include "view.h"

typedef void (*textinput_callback)(char*);

static textinput_callback textcallback;
static char commentbuffer[100];
static int buffersize = 100;
static int commentindex;

static void resetbuffer()
{
	memset(commentbuffer, 0, buffersize);
	commentindex = 0;
}

static void finishcomment_cb(char* comment)
{
	int comstart = min(commentstart, offsetfromxy(cursx, cursy));
	int comend = max(commentstart, offsetfromxy(cursx, cursy));
	comment_addcomment(comstart, comend - comstart, comment);
}

static char* input_clonebuffer()
{
	char* clone = malloc(commentindex);
	strcpy(clone, commentbuffer);
	return clone;
}

void input_setup()
{
	state = 0;
	resetbuffer();
}

void input_starttextinput(textinput_callback callback)
{
	textcallback = callback;
}

static int textinput(char c)
{
	if (
		(c >= 'A' && c <= 'Z') || 
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9') ||
		(c == '.' || c == ',' || c == ' ')
	) {
		commentbuffer[commentindex++] = c;
		commentindex = min(commentindex, 99);
	}

	if (c == 127 || c == 8)
	{
		commentbuffer[--commentindex] = '\0';
	}

	if (c == KEY_ENTER || c == '\n' || c == '\r')
	{
		(textcallback)(input_clonebuffer());
		resetbuffer();
		return state = 0;
	}

	if (c == 27)
	{
		resetbuffer();
		return state = 0;
	}
}

static void normalmodeinput(int ch)
{
	switch (ch)
	{
		case 'h': case KEY_LEFT: movecurs(-1, 0); break;
		case 'H': movecurs(-bytesperline / 2, 0); break;

		case 'j': case KEY_DOWN: movecurs(0, 1); break;
		case 'J': movecurs(0, 5); break;

		case 'k': case KEY_UP: movecurs(0, -1); break;
		case 'K': movecurs(0, -5); break;

		case 'l': case KEY_RIGHT: movecurs(1, 0); break;
		case 'L': movecurs(bytesperline / 2, 0); break;

		case 'x': case 'X':
		{
			comment_t* comment = comment_at(cursy * bytesperline + cursx);
			if (comment)
				comment_delete(comment);
		}
			break;

		case 'c': case 'C':
			if (state == 0)
			{
				if (!comment_at(offsetfromxy(cursx, cursy)))
				{
					state = 1;
					commentstart = offsetfromxy(cursx, cursy);
					input_starttextinput(finishcomment_cb);
				}
			}
			else if (state == 1)
			{
				if (!comment_overlapping(
					min(commentstart, offsetfromxy(cursx, cursy)),
					max(commentstart, offsetfromxy(cursx, cursy))))
				{
					state = 2;
				}
			}
			break;

		case 'w': case 'W':
			if (getch() == 'Y')
			{
				writeannotfile("annot.testfile");
			}
			break;

		case KEY_ENTER:
		case '\n':
		case '\r':
			if (state == 1)
			{
				state = 2;
			}
			break;

		case 27:
			if (state == 1)
			{
				state = 0;
			}
			break;

		case 'q':
		case 'Q':
			run = 0;
			break;
	}
}

void input_update()
{
	switch (state)
	{
		case 2:
			textinput(getch());
			break;
		case 0:
		case 1:
			normalmodeinput(getch());
			break;
	}

	comment_highlighted = comment_at(offsetfromxy(cursx, cursy));
}

void input_draw()
{
	if (state == 2)
	{
		attron(COLOR_PAIR(3));
		mvwprintw(stdscr, 0, 0, "comment: %s", commentbuffer);
		attroff(COLOR_PAIR(3));
	}

	refresh();
}
