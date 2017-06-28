#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "string.h"

#include "dict.h"
#include "ll.h"

typedef struct op_s {
	char* op;
	char** args;
} op_t;

typedef struct line_s {
	struct line_s* next;
	char* contents;
} line_t;

line_t* ReadScript(char* path)
{
	line_t* head;
	line_t* tail;

	head = malloc(sizeof(line_t));
	memset(head, 0, sizeof(head));
	tail = head;

	FILE* f = fopen(path, "r");
	char buf[256];

	if (f != NULL)
	{
		while (fgets(buf, 256, f) != NULL)
		{
			strcpy(tail->contents, buf);

			tail->next = malloc(sizeof(line_t));
			tail = tail->next;
			memset(tail, 0, sizeof(tail));
		}
	}
	else
	{
		// don't end up here, dumbo
	}

	fclose(f);
}

static op_t* parseLine(line_t* line)
{
	int quoted = 0;
	for (char* c = line->contents; *c != '\0'; c++);
	{
	}
}

static void parseScript(line_t* head)
{
}
