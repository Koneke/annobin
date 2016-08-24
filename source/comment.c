#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "comment.h"
#include "common.h"
#include "input.h" // THIS SHOULD NOT BE HERE BAD DESIGN, BAD
#include "model.h"

comment_t* comment_at(int position)
{
	comment_t* current = head;

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

	comment_t* current = head;
	comment_t* ahead = NULL;
	// we leave this when we hit END or
	// find a comment AFTER the position we're inserting at
	while (current && current->position < position)
	{
		ahead = current;
		current = current->next;
	}

	if (current) ahead = current;

	// no current, and an ahead behind us means we're tail now
	if (ahead && ahead->position < position)
	{
		ahead = NULL;
	}

	// ahead should now be the first comment that should be
	// after ours, OR NULL if there was none.
	if (ahead)
	{
		new->index = ahead->index;
		bumpcomments(ahead);

		if (ahead->prev)
		{
			new->prev = ahead->prev;
			ahead->prev->next = new;
		}
		else // we're head now
		{
			head = new;
		}

		ahead->prev = new;
		new->next = ahead;
	}
	else
	{
		if (tail)
		{
			tail->next = new;
			new->prev = tail;
			new->index = tail->index + 1;
			tail = new;
		}
		else
		{
			new->index = 0;
			tail = new;
		}
	}

	new->comment = comment;
	new->position = position;
	new->length = length;

	if (!head) head = new;

	return new;
}

void comment_delete(comment_t* comment)
{
	shrumpcomments(comment); // bump indexes down one
	if (comment->prev) comment->prev->next = comment->next;
	if (comment->next) comment->next->prev = comment->prev;
	if (comment == head) head = comment->next;
	if (comment == tail) tail = comment->prev;
}

void comment_freeall()
{
	comment_t* current = head;
	while (head)
	{
		current = head->next;
		free(head);
		head = current;
	}
}
