#include <stdio.h>
#include "comment.h"
#include "file.h"

#define MODEL_BUFFER_SIZE 0x1000

int model_displayMode; // standard or translationtable
uint8_t* model_buffer;
int model_bufferoffset; // byte
int model_bufferSize; // size of current read in buffer (i.e. < MODEL_BUFFER_SIZE close to EOF)
int model_translationTable[0xff];

comment_t* comment_highlighted;
int model_selectionstart;
int model_selectionend;
int model_selectionlength;
int model_cursoroffset;

int model_selection_firstOffset();
int model_selection_lastOffset();
int model_selection_length();
int model_selection_isOverlappingComments();
void model_setup();
void model_quit();
