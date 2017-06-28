#include <stdio.h>

#include "app.h"
#include "dict.h"

int main(int argc, char** argv)
{
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
