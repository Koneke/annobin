#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "common.h"
#include "cursor.h"
#include "file.h"
#include "view.h"
#include "app.h"

#define BUFFER_SIZE 100

typedef void (*textinput_callback)(char*);

static int inputstate;

static textinput_callback textcallback;

static char inputbuffer[100];
static int inputindex;

static void resetbuffer()
{
	memset(inputbuffer, 0, BUFFER_SIZE);
	inputindex = 0;
}

static void finishcomment_cb(char* comment)
{
	comment_addcomment(model_selectionstart, model_selectionlength, comment);
}

static char* input_clonebuffer()
{
	char* clone = malloc(inputindex);
	strcpy(clone, inputbuffer);
	return clone;
}

static int setstate(int _state)
{
	switch (_state)
	{
		case 0:
			model_selectionstart = -1;
			model_selectionend = -1;
			model_selectionlength = -1;
	}

	return inputstate = _state;
}


void input_setup()
{
	setstate(0);
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
		inputbuffer[inputindex++] = c;
	}

	if (c == 127 || c == 8)
	{
		inputbuffer[--inputindex] = '\0';
	}

	inputindex = clamp(inputindex, 0, BUFFER_SIZE - 1);

	if (c == KEY_ENTER || c == '\n' || c == '\r')
	{
		(textcallback)(input_clonebuffer());
		resetbuffer();
		return setstate(0);
	}

	if (c == 27)
	{
		resetbuffer();
		return setstate(0);
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
			comment_t* comment = comment_at(model_cursoroffset);
			if (comment)
				comment_delete(comment);
		}
			break;

		case 'c': case 'C':
			if (inputstate == 0)
			{
				if (!comment_at(model_cursoroffset))
				{
					setstate(1);
					model_selectionstart = model_cursoroffset;
					input_starttextinput(finishcomment_cb);
				}
			}
			else if (inputstate == 1)
			{
				if (!comment_overlapping(model_selectionstart, model_selectionend))
				{
					setstate(2);
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
			if (inputstate == 1)
			{
				setstate(2);
			}
			break;

		case 27:
			if (inputstate == 1)
			{
				setstate(0);
			}
			break;

		case 'q':
		case 'Q':
			app_running = 0;
			break;
	}
}

static void updateselection()
{
	model_selectionend = model_cursoroffset;

	if (model_selectionend < model_selectionstart)
	{
		int temp = model_selectionstart;
		model_selectionstart = model_selectionend;
		model_selectionend = temp;
	}

	model_selectionlength = model_selectionend - model_selectionstart;
	comment_highlighted = comment_at(model_cursoroffset);
}

void input_update()
{
	switch (inputstate)
	{
		case 2:
			textinput(getch());
			break;
		case 0:
		case 1:
			normalmodeinput(getch());
			break;
	}

	updateselection();
}

void input_draw()
{
	if (inputstate == 2)
	{
		attron(COLOR_PAIR(3));
		mvwprintw(stdscr, 0, 0, "comment: %s", inputbuffer);
		attroff(COLOR_PAIR(3));
	}

	refresh();
}
