#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "common.h"
#include "file.h"
#include "view.h"
#include "app.h"
#include "chartools.h"

#define BUFFER_SIZE 100

typedef enum e_inputstate_e {
	inputstate_normal = 0,
	inputstate_select = 1,
	inputstate_text = 2
} e_inputstate;

typedef enum e_inputmask_e {
	inputmask_none = 0,
	inputmask_letters = 1,
	inputmask_whitespace = 2,
	inputmask_punctuation = 4,
	inputmask_numbers = 8,
	inputmask_hex = 16
} e_inputmask;

typedef void (*textinput_callback)(char*);

static e_inputstate inputstate;
static textinput_callback textcallback;
static int inputmask;
static char inputbuffer[100];
static int inputindex;

static void resetbuffer()
{
	memset(inputbuffer, 0, BUFFER_SIZE);
	inputindex = 0;
}

static void goto_cb(char* address)
{
	unsigned int offset;
	sscanf(address, "%x", &offset);
	view_cursor_setOffset(offset);
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

void input_starttextinput(textinput_callback callback, int mask)
{
	textcallback = callback;
	inputmask = mask;
	setstate(inputstate_text);
	refresh();
}

static int textinput(char c)
{
	if (c == 127 || c == 8)
	{
		inputbuffer[--inputindex] = '\0';
		inputindex = clamp(inputindex, 0, BUFFER_SIZE - 1);
	}
	else if (
		!inputmask ||
		(isLetter(c) && inputmask & inputmask_letters) ||
		(isNumber(c) && inputmask & inputmask_numbers) ||
		(isHex(c) && inputmask & inputmask_hex) ||
		(isPunctuation(c) && inputmask & inputmask_punctuation) ||
		(isWhitespace(c) && inputmask & inputmask_whitespace))
	{
		inputbuffer[inputindex++] = c;
	}

	inputindex = clamp(inputindex, 0, BUFFER_SIZE - 1);

	if (c == KEY_ENTER || c == '\n' || c == '\r')
	{
		(textcallback)(input_clonebuffer());
		resetbuffer();
		return setstate(inputstate_normal);
	}

	if (c == 27)
	{
		resetbuffer();
		return setstate(inputstate_normal);
	}
}

static void selectmodeinput(int ch)
{
	int dx = 0, dy = 0;
	switch (ch)
	{
		case 'h': case KEY_LEFT: dx = -1; dy = 0; break;
		case 'H': dx = -view_bytesperline / 2; dy = 0; break;

		case 'j': case KEY_DOWN: dx = 0; dy = 1; break;
		case 'J': dx = 0; dy = 5; break;

		case 'k': case KEY_UP: dx = 0; dy = -1; break;
		case 'K': dx = 0; dy = -5; break;

		case 'l': case KEY_RIGHT: dx = 1; dy = 0; break;
		case 'L': dx = view_bytesperline / 2; dy = 0; break;

		case KEY_ENTER:
		case '\n':
		case '\r':
		case 'c': case 'C':
			if (!model_selection_isOverlappingComments())
			{
				input_starttextinput(finishcomment_cb, inputmask_none);
			}
			break;

		case 27: // escape
			setstate(inputstate_normal);
			break;
	}

	view_cursor_move(dx, dy);
}

static void gotoNextComment()
{
	comment_t* comment = comment_head;

	while (comment)
	{
		if (comment->position > model_cursoroffset)
		{
			model_cursoroffset = comment->position;
			return;
		}

		comment = comment->next;
	}
}

static void gotoPreviousComment()
{
	comment_t* comment = comment_tail;

	while (comment)
	{
		if (comment->position < model_cursoroffset)
		{
			model_cursoroffset = comment->position;
			return;
		}

		comment = comment->prev;
	}
}

static void normalmodeinput(int ch)
{
	int dx = 0, dy = 0;

	switch (ch)
	{
		case 'h': case KEY_LEFT: dx = -1; dy = 0; break;
		case 'H': dx = -view_bytesperline / 2; dy = 0; break;

		case 'j': case KEY_DOWN: dx = 0; dy = 1; break;
		case 'J': dx = 0; dy = 5; break;

		case 'k': case KEY_UP: dx = 0; dy = -1; break;
		case 'K': dx = 0; dy = -5; break;

		case 'l': case KEY_RIGHT: dx = 1; dy = 0; break;
		case 'L': dx = view_bytesperline / 2; dy = 0; break;

		case 'n': gotoNextComment(); break;
		case 'p': gotoPreviousComment(); break;

		case 'x': case 'X':
		{
			comment_t* comment = comment_at(model_cursoroffset);
			if (comment)
			{
				comment_delete(comment);
			}
		}
			break;

		case 'g': case 'G':
			input_starttextinput(goto_cb, inputmask_hex);
			break;

		case 'c': case 'C':
			if (!comment_at(model_cursoroffset))
			{
				setstate(inputstate_select);
				model_selectionstart = model_cursoroffset;
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

	view_cursor_move(dx, dy);
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
