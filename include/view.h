int view_width, view_height;

int screenscroll;
int bytesperline;
int bytescroll;

void view_setup();
void view_update();
int offsetfromxy(int x, int y);
char getprintchar(char c);

int view_cursorx;
int view_cursory;
