#include "scop.h" 

float cameraPosZ = 0;
extern float ObjectSize;

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
    if (!(source = malloc(sizeof(char) * (fileLength + 1))))
        return (0);
    source[fileLength] = 0;
    read(fd, source, fileLength);
    close(fd);
    return (source);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        cameraPosZ += 0.2 * ObjectSize;
    else if (yoffset < 0)
        cameraPosZ -= 0.2 * ObjectSize;
}

void printUsage() {
    printf("Usage  : ./scop [OPTION]... FILE.obj\n\n");
    printf("option :\n");
    printf("  -t  TEXTURE_FILE\n");
    printf("  -b  BACKGROUND_TEXTURE_FILE\n");
    printf("  -smooth\n");
}

void freeMeshData(t_mesh *mesh) {
    free(mesh->vertices.data);
    free(mesh->indices.data);
    free(mesh->normales.data);
    free(mesh->uvs.data);
    for (int n = 0; n < mesh->segmentNb; n++) {
        free(mesh->segments[n].texture);
        free(mesh->segments[n].name);
    }
    free(mesh->segments);
}

void freeAll(t_scop *scop) {
    glDeleteTextures(1, &scop->background.textureID);
    glDeleteTextures(1, &scop->textures.defaultTextureID);
    for (int n = 0; n < scop->object.segmentNb; n++) {
        glDeleteVertexArrays(1, &scop->object.segments[n].VAO);
        glDeleteTextures(1, &scop->object.segments[n].textureID);
        glDeleteTextures(1, &scop->object.segments[n].normalTextureID);
        glDeleteTextures(1, &scop->object.segments[n].metalTextureID);
        glDeleteTextures(1, &scop->object.segments[n].routhTextureID);
    }
    glDeleteVertexArrays(1, &scop->background.VAO);
    glDeleteProgram(scop->background.programShader);
    glDeleteProgram(scop->object.programShader);
    free(scop->object.segments);
    free(scop->textures.object);
}

void showFPS(GLFWwindow *window) {
    static double oldTime = 0;
    static double newTime;
    static int frames = 0;
    static char title[40];
    double timeDiff;

    newTime = glfwGetTime();
    timeDiff = newTime - oldTime;
    frames++;
    if (timeDiff < 1.0f / 30.0f)
        return ;
    sprintf(title, "scop :  FPS = %d  ms = %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames);
    glfwSetWindowTitle(window, title);
    frames = 0;
    oldTime = newTime;
}