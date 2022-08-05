#include "scop.h" 

char *getShaderSource(char *fileName) {
    char    *source;
    int     fd;
    size_t  fileLength;

    if ((fd = open(fileName, O_RDONLY)) <= 0)
        return (0);
    fileLength = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if (!(source = calloc(sizeof(char), fileLength)))
        return (0);
    read(fd, source, fileLength);
    close(fd);
    return (source);
}