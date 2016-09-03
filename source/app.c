#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "draw.h"
#include "file.h"
#include "input.h"
#include "view.h"
#include "app.h"

#define BUFFER_SIZE 20000

void app_setup(int argc, char** argv)
{
	// make sure to setup model before file,
	// since file expects there to be a buffer already.
	model_setup(BUFFER_SIZE);
	file_setup(argv[1], argv[2]);
	draw_setup();
	input_setup();
	view_setup();
}

void app_run()
{
	app_running = 1;
	while (app_running)
	{
		draw_draw();
		input_draw();
		input_update();
		model_update();
		view_update();
	}
}

void app_quit()
{
	model_quit();
	draw_quit();
	file_quit();
}
