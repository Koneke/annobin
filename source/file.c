#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define _BSD_SOURCE
#define __USE_BSD
#include <endian.h>

#include <message.h>

#include "file.h"
#include "model.h"

static void readannotfile(char* path)
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
		{
			comment[i] = fgetc(annot);
		}

		comment_addcomment(be64toh(position), be64toh(length), comment);
	}

	fclose(annot);
}

static void readTranslationFile(char* path)
{
	translationFile = fopen(path, "r");
	fseek(translationFile, 0, SEEK_SET);

	char dummyBuff[512];
	int from;
	int to;

	for (int i = 0; i < 0xff; i++) model_translationTable[i] = i;

	while (!feof(translationFile))
	{
		char c = fgetc(translationFile);
		if (';' == c)
		{
			// just disregard the comment
			fgets(dummyBuff, 512, translationFile);
		}
		else
		{
			// unread our first char
			fseek(translationFile, -1, SEEK_CUR);
		}

		if (fscanf(translationFile, "%x,%x ", &from, &to) < 2)
		{
			break;
		}

		model_translationTable[from] = to;
	}

	fclose(translationFile);
}

void file_setup(char* filepath, char* annotpath, char* translationPath)
{
	file = fopen(filepath, "r");
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	readannotfile(annotpath);

	if (translationPath)
	{
		readTranslationFile(translationPath);
	}

	file_readintomodelbuffer();
}

void file_readintomodelbuffer()
{
	fread(model_buffer, 1, MODEL_BUFFER_SIZE, file);
}

void file_setOffset(int offset)
{
	fseek(file, offset, SEEK_SET);
}

void file_moveoffset(int offset)
{
	fseek(file, offset, SEEK_CUR);
}

void writeannotfile(char* path)
{
	annot = fopen(path, "w+");

	comment_t* current = comment_head;
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

	message_important("wrote to %s", path);
}

void file_quit()
{
	fclose(file);
}
