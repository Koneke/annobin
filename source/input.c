#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "common.h"
#include "message.h"
#include "draw.h"
#include "app.h"
#include "chartools.h"

#define BUFFER_SIZE 100

static char prompt[50];

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
static char findMode;

static void resetbuffer()
{
	memset(inputbuffer, 0, BUFFER_SIZE);
	inputindex = 0;
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

void input_starttextinput(const char* inputPrompt, textinput_callback callback, int mask)
{
	textcallback = callback;
	inputmask = mask;
	strcpy(prompt, inputPrompt);
	setstate(inputstate_text);
	refresh();
}

static void finishcomment_cb(char* comment)
{
	comment_addcomment(
		model_selection_firstOffset(),
		model_selection_length(),
		comment);
	setstate(inputstate_normal);
}

static void findHex(char* string)
{
}

static void findChar(char* string)
{
}

static void findTranslated(char* string)
{
}

static void find_cb(char* string)
{
	switch (findMode)
	{
		case 'h': findHex(string); break;
		case 'c': findChar(string); break;
		case 't': findTranslated(string); break;
	}

	setstate(inputstate_normal);
}

static void findMode_cb(char* mode)
{
	findMode = 0;

	switch (mode[0])
	{
		case 'h': case 'H': findMode = 'h'; break;
		case 'c': case 'C': findMode = 'c'; break;
		case 't': case 'T': findMode = 't'; break;
	}

	if (findMode)
	{
		char p[] = "Find (#) (prepend ? for upwards)";
		p[6] = findMode;
		input_starttextinput(p, find_cb, inputmask_none);
	}
}

static char* input_clonebuffer()
{
	char* clone = malloc(inputindex);
	strcpy(clone, inputbuffer);
	return clone;
}

void input_setup()
{
	setstate(inputstate_normal);
	resetbuffer();
}

static void textinput(char c)
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
	}

	if (c == 27)
	{
		resetbuffer();
		setstate(inputstate_normal);
	}
}

void updateView()
{
	int magic = model_cursoroffset - model_bufferoffset; // buf rel cursor
	int cursorY = (magic - (magic % view_bytesperline)) / view_bytesperline; // find y buf rel
	view_bytescroll = max(cursorY, view_height / 2) - view_height / 2;
	view_bytescroll *= view_bytesperline;
}

static int checkModelBufferRange()
{
	int lastOffset = model_bufferoffset;
	int linejump = 0x40;
	int margin = view_bytesperline * linejump;

	int moved = 0;

	while (model_cursoroffset - model_bufferoffset > MODEL_BUFFER_SIZE - margin)
	{
		model_bufferoffset += margin * 2;
		moved = 1;
	}

	while (model_bufferoffset > 0 && (model_cursoroffset - model_bufferoffset) < margin)
	{
		model_bufferoffset -= margin * 2;
		model_bufferoffset = max(model_bufferoffset, 0);

		if (model_bufferoffset != lastOffset)
		{
			moved = 1;
		}
	}

	if (moved)
	{
		file_setOffset(model_bufferoffset);
		file_readintomodelbuffer();
		updateView();
	}
}

static void moveCursor(int dx, int dy)
{
	model_cursoroffset += dx + dy * view_bytesperline;
	model_cursoroffset = max(model_cursoroffset, 0);
	model_cursoroffset = min(model_cursoroffset, file_size - 1);

	model_selectionend = model_cursoroffset;
	model_selectionlength = model_selection_lastOffset() - model_selection_firstOffset();
	comment_highlighted = comment_at(model_cursoroffset);

	checkModelBufferRange();
	updateView();
}

static void goto_cb(char* address)
{
	unsigned int offset;
	sscanf(address, "%x", &offset);
	model_cursoroffset = offset;
	checkModelBufferRange();
	setstate(inputstate_normal);
}

static void selectmodeinput(int ch)
{
	switch (ch)
	{
		case KEY_ENTER:
		case '\n':
		case '\r':
		case 'c': case 'C':
			if (!model_selection_isOverlappingComments())
			{
				input_starttextinput("Comment", finishcomment_cb, inputmask_none);
			}
			break;

		case 27: // escape
			setstate(inputstate_normal);
			break;
	}
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

static void anyModeInput(int ch)
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

		case '+': view_bytesperline++; break;
		case '-': view_bytesperline--; break;

		case 'i': view_byteOffset++; break;
		case 'u': view_byteOffset--; break;

		case 't': case 'T': model_displayMode = !model_displayMode; break;
	}

	moveCursor(dx, dy);
}

static void normalmodeinput(int ch)
{
	switch (ch)
	{
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
			input_starttextinput("Goto address", goto_cb, inputmask_hex);
			break;

		case 'f': case 'F':
			input_starttextinput("Find [h]ex, [c]har table, [t]ranslated", findMode_cb, inputmask_letters);
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

		case 'Q':
			if ('y' == getch()) app_running = 0;
			break;
	}
}

static void selectmodedraw()
{
}

static void textmodedraw()
{
	attron(COLOR_PAIR(3));

	for (int i = 0; i < 3; i++)
	{
		mvwprintw(stdscr, view_height / 2 - 1 + i, 10, "%*s", view_width - 20, "");
	}

	mvwprintw(stdscr, view_height / 2, 11, "%s: %s", prompt, inputbuffer);
	attroff(COLOR_PAIR(3));
}

void input_update()
{
	char ch = getch();

	if (inputstate != inputstate_text)
	{
		anyModeInput(ch);
	}

	switch (inputstate)
	{
		case inputstate_normal: normalmodeinput(ch); break;
		case inputstate_select: selectmodeinput(ch); break;
		case inputstate_text: textinput(ch); break;
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
