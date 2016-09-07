// everything with access to view also has access to model by definition
#include "model.h"

int view_width, view_height;
int view_bytesperline;
int view_bytescroll;

void draw_setup();
void draw_postSetup();
void draw_quit();
void draw_draw();
