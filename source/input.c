#include <stdint.h>

#include <ncurses.h>

#include "draw.h"
#include "global.h"
#include "common.h"
#include "comment.h"
#include "cursor.h"
#include "file.h"

int commentinput(char c)
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
		finishcomment();
		commentindex = 0;
		return state = 0;
	}

	if (c == 27)
	{
		commentindex = 0;
		return state = 0;
	}
}

void input()
{
	int ch = getch();

	if (state == 2)
	{
		if (commentinput(ch)) return;
	}

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
			comment_t* comment = commentat(cursy * bytesperline + cursx);
			if (comment)
				deletecomment(comment);
		}
			break;

		case 'c': case 'C':
			if (state == 0)
			{
				if (!commentat(offsetfromxy(cursx, cursy)))
				{
					begincomment();
				}
			}
			else if (state == 1)
			{
				int overlapping = 0;
				for (int i = commentstart; i < offsetfromxy(cursx, cursy); i++)
					overlapping += commentat(i) ? 1 : 0;

				if (!overlapping)
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

		case 27: // escape:
			if (state == 1) state = 0;
			break;

		case 'q':
		case 'Q':
			run = 0;
			break;
	}
}
