void splitFile(char *file);
void appendFile(char *fp, char *body);
void appendZeros(FILE *fd, unsigned int size);
u32 appendImage(char *szb, char *file, char *filename, char *partname, u32 offset, int index);
