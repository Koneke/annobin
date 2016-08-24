#include <stdint.h>

#include "global.h"
#include "common.h"

void validatecurs()
{
	if (cursx < 0)
	{
		if (cursy > 0)
		{
			while (cursx < 0)
			{
				cursx += bytesperline;
				cursy -= 1;
			}
		}
		else
		{
			cursx = 0;
		}
	}
	else if (cursx >= bytesperline)
	{
		while (cursx >= bytesperline)
		{
			cursx -= bytesperline;
			cursy += 1;
		}
	}

	if (cursy < 0) cursy = 0;

	screenscroll = max(cursy, h / 2) - h / 2;
	bytescroll = screenscroll * bytesperline;
}

void movecurs(int x, int y)
{
	cursx += x;
	cursy += y;

	validatecurs();
}

