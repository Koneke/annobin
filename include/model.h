#include "comment.h"

uint8_t* model_buffer;
int model_buffersize;
int model_bufferoffset;

comment_t* comment_highlighted;
int model_selectionstart;
int model_selectionend;
int model_selectionlength;
int model_cursoroffset;
int model_selection_firstOffset();
int model_selection_lastOffset();
int model_selection_length();
int model_selection_isOverlappingComments();
void model_setup(int buffersize);
void model_update();
void model_quit();
