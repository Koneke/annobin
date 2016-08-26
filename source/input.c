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

typedef enum e_inputstate_e {
	inputstate_normal = 0,
	inputstate_select = 1,
	inputstate_text = 2
} e_inputstate;

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
	comment_addcomment(
		model_selection_firstOffset(),
		model_selection_length(),
		comment);
}

static char* input_clonebuffer()
{
	char* clone = malloc(inputindex);
	strcpy(clone, inputbuffer);
	return clone;
}

static int setstate(e_inputstate _state)
{
	switch (_state)
	{
		case inputstate_normal:
			model_selectionstart = -1;
			model_selectionend = -1;
			model_selectionlength = -1;
	}

	return inputstate = _state;
}

void input_setup()
{
	setstate(inputstate_normal);
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

static void selectmodeinput(int ch)
{
	switch (ch)
	{
		case 'h': case KEY_LEFT: movecurs(-1, 0); break;
		case 'H': movecurs(-view_bytesperline / 2, 0); break;

		case 'j': case KEY_DOWN: movecurs(0, 1); break;
		case 'J': movecurs(0, 5); break;

		case 'k': case KEY_UP: movecurs(0, -1); break;
		case 'K': movecurs(0, -5); break;

		case 'l': case KEY_RIGHT: movecurs(1, 0); break;
		case 'L': movecurs(view_bytesperline / 2, 0); break;

		case 'c': case 'C':
			if (!model_selection_isOverlappingComments())
			{
				setstate(inputstate_text);
			}
			break;

		case KEY_ENTER:
		case '\n':
		case '\r':
			setstate(inputstate_text);
			break;

		case 27: // escape
			setstate(inputstate_normal);
			break;
	}
}

static void normalmodeinput(int ch)
{
	switch (ch)
	{
		case 'h': case KEY_LEFT: movecurs(-1, 0); break;
		case 'H': movecurs(-view_bytesperline / 2, 0); break;

		case 'j': case KEY_DOWN: movecurs(0, 1); break;
		case 'J': movecurs(0, 5); break;

		case 'k': case KEY_UP: movecurs(0, -1); break;
		case 'K': movecurs(0, -5); break;

		case 'l': case KEY_RIGHT: movecurs(1, 0); break;
		case 'L': movecurs(view_bytesperline / 2, 0); break;

		case 'x': case 'X':
		{
			comment_t* comment = comment_at(model_cursoroffset);
			if (comment)
				comment_delete(comment);
		}
			break;

		case 'c': case 'C':
			if (!comment_at(model_cursoroffset))
			{
				setstate(inputstate_select);
				model_selectionstart = model_cursoroffset;
				input_starttextinput(finishcomment_cb);
			}
			break;

		case 'w': case 'W':
			if (getch() == 'Y')
			{
				writeannotfile("annot.testfile");
			}
			break;

		case 'q':
		case 'Q':
			app_running = 0;
			break;
	}
}

static void selectmodedraw()
{
}

static void textmodedraw()
{
	attron(COLOR_PAIR(3));
	mvwprintw(stdscr, view_height / 2, 0, "Comment: %s", inputbuffer);
	attroff(COLOR_PAIR(3));
}

void input_update()
{
	switch (inputstate)
	{
		case inputstate_normal: normalmodeinput(getch()); break;
		case inputstate_select: selectmodeinput(getch()); break;
		case inputstate_text: textinput(getch()); break;
	}
}

void input_draw()
{
	switch (inputstate)
	{
		case inputstate_select: selectmodedraw(); break;
		case inputstate_text: textmodedraw(); break;
	}

	refresh();
}
