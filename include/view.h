// everything with access to view also has access to model by definition
#include "model.h"

int view_cursorx, view_cursory;
int view_width, view_height;

int view_screenscroll;
int view_bytesperline;
int view_bytescroll;

void view_setup();
void view_update();

void cursor_setOffset(int offset);
void movecurs(int x, int y);

// move to model?
void xyfromoffset(int offset, int* x, int* y);
int offsetfromxy(int x, int y);

// static? or to draw rather maybe
char getprintchar(char c);

int view_cursorx;
int view_cursory;
