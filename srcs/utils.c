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

void printUsage() {
    printf("Usage : ./scop [OPTION]... FILE.obj\n");
}

char *getObjectFile(int ac, char **av) {
    int n;
    char *ret;

    n = 0;
    while (++n < ac) {
        ret = strchr(av[n], '.');
        if (ret && !strcmp(ret, ".obj"))
            return (av[n]);
    }
    return (0);
}

char *ft_strjoin(char const *s1, char const *s2)
{
	int		len = strlen(s1);
	char	*str;

	if (!(s1 && s2))
		return (0);
	if (!(str = malloc(sizeof(char) * (len + strlen(s2) + 1))))
		return (0);
    memcpy(str, s1, len);
    strcpy(str + len, s2);
	return (str);
}