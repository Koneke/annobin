#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ll.h"

ll_t* llCreate()
{
	ll_t* new = malloc(sizeof(ll_t));
	new->length = 0;
	return new;
}

void llShatter()
{
}

void llDestroy(ll_t* ll)
{
	lln_t* i = ll->tail;
	lln_t* p;

	while (i)
	{
		p = i->prev;
		free(i->data);
		free(i);
		i = p;
	}
}

void llAdd(ll_t* ll, void* data)
{
	lln_t* new = malloc(sizeof(lln_t));
	memset(new, 0, sizeof(lln_t));
	new->data = data;

	if (ll->tail)
	{
		ll->tail->next = new;
		new->prev = ll->tail;
	}
	else
	{
		ll->head = new;
		ll->tail = new;
	}

	ll->length = ll->length + 1;
}

static lln_t* llGetNode(ll_t* ll, int index)
{
	lln_t* n = ll->head;
	while (index--)
	{
		if (n)
		{
			n = n->next;
		}
	}

	return n;
}

void* llGet(ll_t* ll, int index)
{
	lln_t* n = llGetNode(ll, index);

	if (n)
	{
		return n->data;
	}

	return NULL;
}

unsigned llFindFrom(ll_t* ll, void* element, int from)
{
	for (unsigned i = 0; i < ll->length; i++)
	{
		if (llGet(ll, i) == element)
		{
			return i;
		}
	}
	
	return -1;
}

unsigned llFind(ll_t* ll, void* element)
{
	return llFindFrom(ll, element, 0);
}

static void llRemoveNode(ll_t* ll, lln_t* n)
{
	if (n->next) n->next->prev = n->prev;
	if (n->prev) n->prev->next = n->next;

	free(n);

	ll->length = ll->length - 1;
}

void llRemove(ll_t* ll, int index)
{
	lln_t* n = llGetNode(ll, index);

	if (!n)
	{
		// ??? something shat itself hardcore
	}

	llRemoveNode(ll, n);
}

void llDelete(ll_t* ll, int index)
{
	lln_t* n = llGetNode(ll, index);
	free(n->data);

	llRemove(ll, index);
}
