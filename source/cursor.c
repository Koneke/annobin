#include <stdint.h>

#include "common.h"
#include "view.h"
#include "model.h"

void validatecurs()
{
	if (model_cursorx < 0)
	{
		if (model_cursory > 0)
		{
			while (model_cursorx < 0)
			{
				model_cursorx += bytesperline;
				model_cursory -= 1;
			}
		}
		else
		{
			model_cursorx = 0;
		}
	}
	else if (model_cursorx >= bytesperline)
	{
		while (model_cursorx >= bytesperline)
		{
			model_cursorx -= bytesperline;
			model_cursory += 1;
		}
	}

	if (model_cursory < 0) model_cursory = 0;

	screenscroll = max(model_cursory, view_height / 2) - view_height / 2;
	bytescroll = screenscroll * bytesperline;
}

void movecurs(int x, int y)
{
	model_cursorx += x;
	model_cursory += y;

	validatecurs();

	model_cursoroffset = offsetfromxy(model_cursorx, model_cursory);
}

