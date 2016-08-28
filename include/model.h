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
//int model_cursorx;
//int model_cursory;
int model_cursoroffset;
int model_selection_firstOffset();
int model_selection_lastOffset();
int model_selection_length();
int model_selection_isOverlappingComments();
void model_update();
