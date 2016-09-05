#include <stdlib.h>

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
		if (comment_at(i))
		{
			return 1;
		}
	}

	return 0;
}

void model_setup()
{
	model_buffer = malloc(MODEL_BUFFER_SIZE);
}

void model_quit()
{
	comment_t* current = comment_head;
	comment_t* next = current;

	while (current)
	{
		next = current->next;
		free(current);
		current = next;
	}

	free(model_buffer);
}
