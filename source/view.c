#include <stdint.h>

#include "view.h"

void view_setup()
{
	bytesperline = 16;
	bytescroll = 0;
}

int offsetfromxy(int x, int y)
{
	return y * bytesperline + x;
}

void xyfromoffset(int offset, int* x, int* y)
{
	*x = offset % bytesperline;
	*y = (offset - *x) / bytesperline;
}

void view_update()
{
	view_cursorx = model_cursorx;
	view_cursory = model_cursory - screenscroll;
}
