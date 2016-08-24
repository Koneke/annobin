typedef struct comment_s {
	int index;
	char* comment;
	uint64_t position;
	uint64_t length;
	struct comment_s* prev;
	struct comment_s* next;
} comment_t;

comment_t* commentat(int position);

comment_t* head;
comment_t* tail;

void bumpcomments(comment_t* start);
void shrumpcomments(comment_t* start);
comment_t* addcomment(int position, int length, char* comment);
void finishcomment();
void deletecomment(comment_t* comment);
void begincomment();
void freeallcomments();
