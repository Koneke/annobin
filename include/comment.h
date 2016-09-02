#include <stdint.h>

typedef struct comment_s {
	int index;
	char* comment;
	uint64_t position;
	uint64_t length;
	struct comment_s* prev;
	struct comment_s* next;
} comment_t;

comment_t* comment_head;
comment_t* comment_tail;

comment_t* comment_addcomment(int position, int length, char* comment);
      void comment_delete(comment_t* comment);
comment_t* comment_at(int position);
      void comment_freeall();
