#include "model.h"
#include "common.h"

int model_selection_firstOffset()
{
	return min(model_selectionend, model_selectionstart);
}

int model_selection_lastOffset()
{
	return max(model_selectionend, model_selectionstart);
}

int model_selection_length()
{
	return model_selection_lastOffset() - model_selection_firstOffset();
}

int model_selection_isOverlappingComments()
{
	int start = model_selection_firstOffset();
	int end = model_selection_lastOffset();

	for (int i = start; i <= end; i++)
	{
		if (comment_at(i)) return 1;
	}

	return 0;
}

static void selection_update()
{
	model_selectionend = model_cursoroffset;
	model_selectionlength = model_selection_lastOffset() - model_selection_firstOffset();
	comment_highlighted = comment_at(model_cursoroffset);
}

void model_update()
{
	selection_update();
}
