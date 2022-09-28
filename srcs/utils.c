#include "scop.h" 

float cameraPosZ = 0;

char *getShaderSource(char *fileName) {
    char    *source;
    int     fd;
    size_t  fileLength;

    if ((fd = open(fileName, O_RDONLY)) <= 0) {
        dprintf(2, "Failed to open %s\n", fileName);
        return (0);
    }
    fileLength = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if (!(source = calloc(sizeof(char), fileLength + 1)))
        return (0);
    read(fd, source, fileLength);
    close(fd);
    return (source);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        cameraPosZ += 0.5f;
    else if (yoffset < 0)
        cameraPosZ -= 0.5f;
}