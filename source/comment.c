#include <stdlib.h>

#include "model.h"

comment_t* comment_at(int position)
{
	comment_t* current = comment_head;

	while (current)
	{
		int start = current->position;
		int end = current->position + current->length;

		if (start <= position && end >= position)
		{
			return current;
		}
		else
		{
			current = current->next;
		}
	}

	return NULL;
}

int comment_overlapping(int start, int end)
{
	for (int i = start; i <= end; i++)
	{
		if (comment_at(i)) return 1;
	}

	return 0;
}

static void bumpcomments(comment_t* start)
{
	comment_t* current = start;
	while (current)
	{
		current->index++;
		current = current->next;
	}
}

static void shrumpcomments(comment_t* start)
{
	comment_t* current = start;
	while (current)
	{
		current->index--;
		current = current->next;
	}
}

comment_t* comment_addcomment(int position, int length, char* comment)
{
	comment_t* new = malloc(sizeof(comment_t));

	comment_t* current = comment_head;
	comment_t* next = NULL;
	// we leave this when we hit END or
	// find a comment AFTER the position we're inserting at
	while (current && current->position < position)
	{
		next = current;
		current = current->next;
	}

	if (current) next = current;

	// no current, and an next behind us means we're comment_tail now
	if (next && next->position < position)
	{
		next = NULL;
	}

	// next should now be the first comment that should be
	// after ours, OR NULL if there was none.
	if (next)
	{
		new->index = next->index;
		bumpcomments(next);

		if (next->prev)
		{
			new->prev = next->prev;
			next->prev->next = new;
		}
		else // we're comment_head now
		{
			comment_head = new;
		}

		next->prev = new;
		new->next = next;
	}
	else
	{
		if (comment_tail)
		{
			comment_tail->next = new;
			new->prev = comment_tail;
			new->index = comment_tail->index + 1;
			comment_tail = new;
		}
		else
		{
			new->index = 0;
			comment_tail = new;
		}
	}

	new->comment = comment;
	new->position = position;
	new->length = length;

	if (!comment_head) comment_head = new;

	return new;
}

void comment_delete(comment_t* comment)
{
	shrumpcomments(comment); // bump indexes down one
	if (comment->prev) comment->prev->next = comment->next;
	if (comment->next) comment->next->prev = comment->prev;
	if (comment == comment_head) comment_head = comment->next;
	if (comment == comment_tail) comment_tail = comment->prev;
}
