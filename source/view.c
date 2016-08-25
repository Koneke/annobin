#include <stdint.h>

#include "view.h"
#include "common.h"

void view_setup()
{
	view_bytesperline = 16;
	view_bytescroll = 0;
}

int offsetfromxy(int x, int y)
{
	return y * view_bytesperline + x;
}

void xyfromoffset(int offset, int* x, int* y)
{
	*x = offset % view_bytesperline;
	*y = (offset - *x) / view_bytesperline;
}

void view_update()
{
	view_cursorx = model_cursorx;
	view_cursory = model_cursory - view_screenscroll;

	view_screenscroll = max(model_cursory, view_height / 2) - view_height / 2;
	view_bytescroll = view_screenscroll * view_bytesperline;
}
