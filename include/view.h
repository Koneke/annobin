// everything with access to view also has access to model by definition
#include "model.h"

int view_width, view_height;

int view_screenscroll;
int view_bytesperline;
int view_bytescroll;

void view_setup();
void view_update();
int offsetfromxy(int x, int y);
char getprintchar(char c);

int view_cursorx;
int view_cursory;
