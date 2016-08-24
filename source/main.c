#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define _BSD_SOURCE
#define __USE_BSD
#include <endian.h>
#include <unistd.h>

#include <ncurses.h>

FILE* file;
FILE* annot;

typedef struct comment_s {
	int index;
	char* comment;
	uint64_t position;
	uint64_t length;
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

void draw();

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
	comment_t* ahead = NULL;
	// we leave this when we hit END or
	// find a comment AFTER the position we're inserting at
	while (current && current->position < position)
	{
		ahead = current;
		current = current->next;
	}

	if (current) ahead = current;

	// no current, and an ahead behind us means we're tail now
	if (ahead && ahead->position < position)
	{
		ahead = NULL;
	}

	// ahead should now be the first comment that should be
	// after ours, OR NULL if there was none.
	if (ahead)
	{
		new->index = ahead->index;
		bumpcomments(ahead);

		if (ahead->prev)
		{
			new->prev = ahead->prev;
			ahead->prev->next = new;
		}

		ahead->prev = new;
		new->next = ahead;
	}
	else
	{
		if (tail)
		{
			tail->next = new;
			new->prev = tail;
			new->index = tail->index + 1;
			tail = new;
		}
		else
		{
			new->index = 0;
			tail = new;
		}
	}

	new->comment = comment;
	new->position = position;
	new->length = length;

	if (!head) head = new;

	return new;
}

void finishcomment()
{
	char* comment = malloc(commentindex);
	strcpy(comment, commentbuffer);

	int comstart = min(commentstart, offsetfromxy(cursx, cursy));
	int comend = max(commentstart, offsetfromxy(cursx, cursy));
	addcomment(comstart, comend - comstart, comment);
}

void deletecomment(comment_t* comment)
{
	shrumpcomments(comment); // bump indexes down one
	if (comment->prev) comment->prev->next = comment->next;
	if (comment->next)
	{
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

	return '_';

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
				attron(COLOR_PAIR(1 + (comment->index % 2)));

				if (comment->index > commentswritten)
				{
					int commenty = y;
					while (commenty <= commentlast)
					{
						commenty++;
					}
					mvwprintw(stdscr, commenty, 28 + 3 * bytesperline, comment->comment);
					commentswritten++;
					commentlast = commenty;
				}
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

void readannotfile(char* path)
{
	annot = fopen(path, "r");
	fseek(annot, 0, SEEK_SET);

	uint64_t position, length;
	uint16_t comlength;
	char tempbuffer[128];

	while (!feof(annot))
	{
		if (fread(&position, sizeof(uint64_t), 1, annot) != 1) break;
		if (fread(&length, sizeof(uint64_t), 1, annot) != 1) break;
		if (fread(&comlength, sizeof(uint16_t), 1, annot) != 1) break;

		int cl = be16toh(comlength);
		char* comment = malloc(cl + 1);
		memset(comment, 0, cl + 1);
		for (int i = 0; i < cl + 1; i++)
			comment[i] = fgetc(annot);

		addcomment(be64toh(position), be64toh(length), comment);
	}

	fclose(annot);
}

void writeannotfile(char* path)
{
	annot = fopen(path, "w+");

	comment_t* current = head;
	int count = 1;
	while (current)
	{
		uint64_t position, length;
		position = htobe64(current->position);
		length = htobe64(current->length);

		fwrite(&position, sizeof(uint64_t), 1, annot);
		fwrite(&length, sizeof(uint64_t), 1, annot);

		uint16_t comlength;
		comlength = htobe16(strlen(current->comment));

		fwrite(&comlength, sizeof(uint16_t), 1, annot);

		char* c = current->comment;
		while (*c) {
			fputc(*c, annot);
			c++;
		}
		fputc('\0', annot);

		current = current->next;
	}

	fclose(annot);

	wbkgd(stdscr, COLOR_PAIR(2));
	attron(COLOR_PAIR(5));
	mvwprintw(stdscr, 0, 0, "wrote to %s", path);
	attroff(COLOR_PAIR(5));
	getch();
	wbkgd(stdscr, COLOR_PAIR(0));
	draw();
}

int main(int argc, char** argv)
{
	initscr();
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_RED); // cursor
	init_pair(4, COLOR_RED, COLOR_WHITE); // commenting area
	init_pair(5, COLOR_WHITE, COLOR_RED); // important
	noecho();

	file = fopen("in.testfile", "r");
	readannotfile("annot.testfile");

	/*char* com = malloc(4);
	com = strcpy(com, "foo");
	addcomment(0, 10, com);*/

	getmaxyx(stdscr, h, w);

	bytesperline = 16;
	bytescroll = 0;

	int size = 20000;
	buffer = malloc(size);
	fread(buffer, 1, size, file);

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
