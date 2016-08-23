#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <ncurses.h>

FILE* file;
FILE* annot;

typedef struct comment_s {
	int index;
	char* comment;
	int position;
	int length;
	struct comment_s* prev;
	struct comment_s* next;
} comment_t;

comment_t* head;
comment_t* tail;

int state;
int w, h;
uint8_t* buffer;
int bytesperline;
int bytescroll;

int commentstart;
int screenscroll;
int cursx, cursy;

int commentindex;
char commentbuffer[100];

int min(int a, int b) { return a < b ? a : b; }

int max(int a, int b) { return a > b ? a : b; }

int offsetfromxy(int x, int y)
{
	return y * bytesperline + x;
}

comment_t* commentat(int position)
{
	comment_t* current = head;

	while (current)
	{
		int start = current->position;
		int end = current->position + current->length;

		if (start <= position && end >= position)
		{
			return current;
		}
		else
		{
			current = current->next;
		}
	}

	return NULL;
}

void bumpcomments(comment_t* start)
{
	comment_t* current = start;
	while (current)
	{
		current->index++;
		current = current->next;
	}
}

void shrumpcomments(comment_t* start)
{
	comment_t* current = start;
	while (current)
	{
		current->index--;
		current = current->next;
	}
}

comment_t* addcomment(int position, int length, char* comment)
{
	comment_t* new = malloc(sizeof(comment_t));

	comment_t* current = head;
	while (current && current->position < position)
	{
		current = current->next;
	}

	if (current) bumpcomments(current);

	new->index = tail ? tail->index + 1 : 0;
	new->comment = comment;
	new->position = position;
	new->length = length;
	new->prev = tail;

	if (!head) head = new;
	if (tail) tail->next = new;
	tail = new;

	return new;
}

void finishcomment()
{
	char* comment = malloc(commentindex);
	strcpy(comment, commentbuffer);
	addcomment(
		commentstart,
		offsetfromxy(cursx, cursy) - commentstart,
		comment);
}

void deletecomment(comment_t* comment)
{
	if (comment->prev) comment->prev->next = comment->next;
	if (comment->next)
	{
		shrumpcomments(comment->next); // bump indexes down one
		comment->next->prev = comment->prev;
	}
	if (comment == head) head = comment->next;
	if (comment == tail) tail = comment->prev;
}

void movecurs(int x, int y)
{
	cursx += x;
	cursy += y;

	if (cursx < 0)
	{
		if (cursy > 0)
		{
			while (cursx < 0)
			{
				cursx += bytesperline;
				cursy -= 1;
			}
		}
		else
		{
			cursx = 0;
		}
	}
	else if (cursx >= bytesperline)
	{
		while (cursx >= bytesperline)
		{
			cursx -= bytesperline;
			cursy += 1;
		}
	}

	if (cursy < 0) cursy = 0;

	screenscroll = max(cursy, h / 2) - h / 2;
	bytescroll = screenscroll * bytesperline;
}

char getprintchar(char c)
{
	if (
		(c >= 'A' && c <= 'Z') || 
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9')
	) {
		return c;
	}

	return ' ';

	switch(c)
	{
		case 0:
		case 0x0d: // cr
			return ' ';
	}

	return c;
}

void draw()
{
	int offset;
	int lastcomment = 0; // line (so we don't overlap comments)
	int commentswritten = -1;
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
				attron(COLOR_PAIR(1 + (comment->index % 2)));
				mvwprintw(stdscr, y, 41 + 3 * bytesperline, comment->comment);
			}

			if (state > 0 && offset >= commentstart && offset < offsetfromxy(cursx, cursy))
			{
				attron(COLOR_PAIR(4));
			}

			if (i == cursx && y == (cursy - screenscroll))
			{
				attron(COLOR_PAIR(3));
			}

			mvwprintw(stdscr, y, 8 + i * 3, "%02x ", buffer[offset]);

			move(y, 9 + 3 * bytesperline + 2 * i);
			printw("%c ", getprintchar(buffer[offset]));

			attroff(COLOR_PAIR(1));
			attroff(COLOR_PAIR(2));
		}
		move(y, 0);
	}

	refresh();
}

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

	if (c == KEY_BACKSPACE)
	{
		commentbuffer[commentindex--] = '\0';
	}

	if (c == KEY_ENTER || c == '\n' || c == '\r')
	{
		finishcomment();
		return state = 0;
	}

	if (c == 27)
	{
		return state = 0;
	}
}

int main(int argc, char** argv)
{
	initscr();
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_RED); // cursor
	init_pair(4, COLOR_RED, COLOR_WHITE); // commenting
	noecho();

	file = fopen("in.file", "r");
	annot = fopen("annot.file", "r+");

	getmaxyx(stdscr, h, w);

	bytesperline = 16;
	bytescroll = 0;

	int size = 20000;
	buffer = malloc(size);
	fread(buffer, 1, size, file);

	char* com = malloc(4);
	com = strcpy(com, "foo");
	addcomment(0, 10, com);
	com = malloc(4);
	com = strcpy(com, "bar");
	addcomment(20, 3, com);

	printw("h %i", h/2);

	int run = 1;
	state = 0; // 0 browse, 1 insert, 2 write
	while (run)
	{
		draw();
		int ch = getch();

		if (state == 2)
		{
			if (commentinput(ch)) continue;
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
						state = 1;
						strcpy(commentbuffer, "");
						commentstart = cursy * bytesperline + cursx;
					}
				}
				else if (state == 1)
				{
					int overlapping = 0;
					for (int i = commentstart; i < offsetfromxy(cursx, cursy); i++)
						overlapping += commentat(i) ? 1 : 0;

					if (!overlapping)
						state = 2;
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

		if (bytescroll < 0)
		{
			bytescroll = 0;
		}
	}

	comment_t* current = head;
	while (head)
	{
		current = head->next;
		free(head);
		head = current;
	}

	free(buffer);
	endwin();

	return 0;
}
