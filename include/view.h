// everything with access to view also has access to model by definition
#include "model.h"

int view_cursorx, view_cursory;
int view_width, view_height;

int view_bytesperline;
int view_bytescroll;

void view_setup();
void view_update();

void view_cursor_setOffset(int offset);
void view_cursor_move(int x, int y);

int view_cursorx;
int view_cursory;
