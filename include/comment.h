#include <stdint.h>

typedef struct comment_s {
	int index;
	char* comment;
	uint64_t position;
	uint64_t length;
	struct comment_s* prev;
	struct comment_s* next;
} comment_t;

int commentstart;
comment_t* head;
comment_t* tail;
comment_t* comment_highlighted;

      void comment_finishcomment_cb(char* comment);
comment_t* comment_addcomment(int position, int length, char* comment);
      void comment_delete(comment_t* comment);
comment_t* comment_at(int position);
       int comment_overlapping(int start, int end);
      void comment_freeall();
