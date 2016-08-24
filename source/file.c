#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define _BSD_SOURCE
#define __USE_BSD
#include <endian.h>

#include <curses.h>

#include "file.h"
#include "model.h"

void file_setup(char* filepath, char* annotpath)
{
	file = fopen("in.testfile", "r");
	readannotfile("annot.testfile");
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

		comment_addcomment(be64toh(position), be64toh(length), comment);
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
}
