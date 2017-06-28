#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

#define HASHSIZE 1000

struct HashTable_s {
	char* Elements[HASHSIZE];
};

typedef struct HashTableIndex_s {
	struct HashTableIndex_s* Next;
	struct HashTableIndex_s* Prev;
	HashTable_t* Table;
} HashTableIndex_t;

static HashTableIndex_t* Head;
static HashTableIndex_t* Tail;

HashTable_t* TableCreate()
{
	HashTableIndex_t* temp = malloc(sizeof(HashTableIndex_t));
	temp->Table = malloc(sizeof(HashTable_t));
	memset(temp->Table->Elements, 0, sizeof(temp->Table->Elements));
	temp->Prev = Tail;

	if (Tail)
	{
		Tail->Next = temp;
	}

	Tail = temp;

	if (!Head)
	{
		Head = Tail;
	}

	return temp->Table;
}

void TableDestroy(HashTable_t* table)
{
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
