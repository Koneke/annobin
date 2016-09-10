FILE* file;
int file_size;
char* file_annotationFilePath;

int file_setup(char* filepath, char* annotpath, char* translationPath);
void file_quit();
void file_setOffset(int offset);
void file_moveoffset(int offset);
void file_readintomodelbuffer();
void writeannotfile(char* path);
