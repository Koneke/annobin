#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "ll.h"

#define HASHSIZE 1000

struct HashTable_s {
	char* Elements[HASHSIZE];
};

static ll_t tables;

HashTable_t* TableCreate()
{
	HashTable_t* t = malloc(sizeof(HashTable_t));
	llAdd(&tables, t);

	return t;
}

void TableDestroy(HashTable_t* table)
{
	unsigned idx = llFind(&tables, table);

	if (idx > 0)
	{
		HashTable_t* t = llGet(&tables, idx);

		for (unsigned j = 0; j < HASHSIZE; j++)
		{
			if (t->Elements[j])
			{
				free(t->Elements[j]);
			}
		}

		llRemove(&tables, idx);
	}
}

static unsigned hash(char* data)
{
	unsigned hashValue;

	for (hashValue = 0; *data != '\0'; data++)
	{
		hashValue = *data + 31 * hashValue;
	}

	return hashValue % HASHSIZE;
}

void TableSet(HashTable_t* table, char* key, char* value)
{
	unsigned keyHash = hash(key);

	free(table->Elements[keyHash]);

	table->Elements[keyHash] = malloc(strlen(value) + 1);
	strcpy(table->Elements[keyHash], value);
}

char* TableGet(HashTable_t* table, char* key)
{
	return table->Elements[hash(key)];
}
