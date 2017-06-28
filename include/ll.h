#pragma once

typedef struct lln_s {
	struct lln_s* next;
	struct lln_s* prev;
	void* data;
} lln_t;

typedef struct ll_s {
	unsigned length;
	lln_t* head;
	lln_t* tail;
} ll_t;



ll_t* llCreate();
void llDestroy(ll_t* ll);

void llAdd(ll_t* ll, void* data);
void* llGet(ll_t* ll, int index);
unsigned llFind(ll_t* ll, void* element);
void llRemove(ll_t* ll, int index); // remove node, dont free data
void llDelete(ll_t* ll, int index); // remove node, free data
