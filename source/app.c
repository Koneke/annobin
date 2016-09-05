#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "draw.h"
#include "input.h"
#include "app.h"

void app_setup(int argc, char** argv)
{
	// make sure to setup model before file,
	// since file expects there to be a buffer already.
	model_setup();
	file_setup(argv[1], argv[2]);
	draw_setup();
	input_setup();
}

void app_run()
{
	app_running = 1;
	while (app_running)
	{
		draw_draw();
		input_draw();
		input_update();
	}
}

void app_quit()
{
	model_quit();
	draw_quit();
	file_quit();
}
