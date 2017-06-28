#include <stdio.h>
#include <stdlib.h>

#include "string.h"

#include "app.h"
#include "dict.h"
#include "ll.h"

int main(int argc, char** argv)
{
	/*ll_t* ll = llCreate();

	char* foo = malloc(sizeof(char) * 10);
	strcpy(foo, "foobar");
	llAdd(ll, foo);

	char* bar = malloc(sizeof(char) * 10);
	strcpy(bar, "quxbaz");
	llAdd(ll, bar);

	printf("%s\n", llGet(ll, 0));
	printf("%s\n", llGet(ll, 1));

	llDestroy(ll);

	return 0;*/

	/*char k[] = "foo";
	char v[] = "bar";
	HashTable_t* t = TableCreate();
	TableSet(t, k, v);

	printf("%s : %s\n", k, TableGet(t, k));

	TableDestroy(t);

	return 0;*/

	if (argc < 3 || argc > 4)
	{
		printf("usage: annobin <file> <annotation file> (optional: <translation file>)\n");
		return 0;
	}

	if (app_setup(argc, argv))
	{
		app_run();
		app_quit();
	}

	return 0;
}
