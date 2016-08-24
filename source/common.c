#include <stdint.h>

#include "global.h"

int min(int a, int b) { return a < b ? a : b; }

int max(int a, int b) { return a > b ? a : b; }

char getprintchar(char c)
{
	if (
		(c >= 'A' && c <= 'Z') || 
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9')
	) {
		return c;
	}

	return '_';

	switch(c)
	{
		case 0:
		case 0x0d: // cr
			return ' ';
	}

	return c;
}
