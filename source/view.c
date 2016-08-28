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

void cursor_setOffset(int offset)
{
	xyfromoffset(offset, &view_cursorx, &view_cursory);
	model_cursoroffset = offset;
}

static void validatecurs()
{
	if (view_cursorx < 0)
	{
		if (view_cursory > 0)
		{
			while (view_cursorx < 0)
			{
				view_cursorx += view_bytesperline;
				view_cursory -= 1;
			}
		}
		else
		{
			view_cursorx = 0;
		}
	}
	else if (view_cursorx >= view_bytesperline)
	{
		while (view_cursorx >= view_bytesperline)
		{
			view_cursorx -= view_bytesperline;
			view_cursory += 1;
		}
	}

	if (view_cursory < 0)
	{
		 view_cursory = 0;
	}
}

void movecurs(int x, int y)
{
	view_cursorx += x;
	view_cursory += y;

	validatecurs();

	model_cursoroffset = offsetfromxy(view_cursorx, view_cursory);

	model_selectionend = model_cursoroffset;
	model_selectionlength = model_selection_lastOffset() - model_selection_firstOffset();
	comment_highlighted = comment_at(model_cursoroffset);
}

void view_update()
{
	view_cursorx = view_cursorx;
	view_cursory = view_cursory - view_screenscroll;

	view_screenscroll = max(view_cursory, view_height / 2) - view_height / 2;
	view_bytescroll = view_screenscroll * view_bytesperline;
}
