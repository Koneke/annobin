FILE* file;
int file_size;
FILE* annot;

void file_setup(char* filepath, char* annotpath);
void file_quit();
void file_setOffset(int offset);
void file_moveoffset(int offset);
void file_readintomodelbuffer();
void readannotfile(char* path);
void writeannotfile(char* path);
