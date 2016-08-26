#include <stdint.h>

#include "common.h"
#include "view.h"

void cursor_setOffset(int offset)
{
	xyfromoffset(offset, &model_cursorx, &model_cursory);
	model_cursoroffset = offset;
}

void validatecurs()
{
	if (model_cursorx < 0)
	{
		if (model_cursory > 0)
		{
			while (model_cursorx < 0)
			{
				model_cursorx += view_bytesperline;
				model_cursory -= 1;
			}
		}
		else
		{
			model_cursorx = 0;
		}
	}
	else if (model_cursorx >= view_bytesperline)
	{
		while (model_cursorx >= view_bytesperline)
		{
			model_cursorx -= view_bytesperline;
			model_cursory += 1;
		}
	}

	if (model_cursory < 0) model_cursory = 0;
}

void movecurs(int x, int y)
{
	model_cursorx += x;
	model_cursory += y;

	validatecurs();

	model_cursoroffset = offsetfromxy(model_cursorx, model_cursory);
}

