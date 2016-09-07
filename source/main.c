#include <stdio.h>

#include "app.h"

int main(int argc, char** argv)
{
	if (argc < 3 || argc > 4)
	{
		printf("usage: annobin <file> <annotation file> (optional: <translation file>)\n");
		return 0;
	}

	app_setup(argc, argv);
	app_run();
	app_quit();

	return 0;
}
