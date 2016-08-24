#include "comment.h"

// rename us!
	uint8_t* buffer;
	int bytescroll;
	comment_t* head;
	comment_t* tail;

comment_t* comment_highlighted;
int model_selectionstart;
int model_selectionend;
int model_selectionlength;
int model_cursorx;
int model_cursory;
int model_cursoroffset;
