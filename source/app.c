#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "draw.h"
#include "comment.h"
#include "file.h"
#include "input.h"
#include "view.h"
#include "model.h"
#include "app.h"

void app_setup(int argc, char** argv)
{
	file_setup(argv[1], argv[2]);
	draw_setup();
	input_setup();
	view_setup();

	int size = 20000;
	buffer = malloc(size);
	fread(buffer, 1, size, file);
}

void app_run()
{
	app_running = 1;
	while (app_running)
	{
		draw_draw();
		input_draw();
		input_update();
		view_update();
	}
}

void app_quit()
{
	comment_freeall();
	free(buffer);
	draw_quit();
}
