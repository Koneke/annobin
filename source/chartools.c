int isInRange(char c, char low, char high)
{
	return c >= low && c <= high;
}

int isLetter(char c)
{
	return isInRange(c, 'A', 'Z') || isInRange(c, 'a', 'z');
}

int isNumber(char c)
{
	return isInRange(c, '0', '9');
}

int isHex(char c)
{
	return isNumber(c) || isInRange(c, 'A', 'F') || isInRange(c, 'a', 'f');
}

int isPunctuation(char c)
{
	return c == ',' || c == '.';
}

int isWhitespace(char c)
{
	return c == ' ';
}
