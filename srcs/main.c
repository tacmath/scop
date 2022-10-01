#include "scop.h" 

int initBackground(t_scop *scop) {

    GLfloat vertices[] =
    { //     COORDINATES   
        -1.0f, -1.0f,  0.0f,
        1.0f, 1.0f,  0.0f,
        -1.0f, 1.0f,  0.0f,
        1.0f, -1.0f,  0.0f,
    };

    GLuint indices[] =
    {
        0, 1, 2,
        0, 1, 3,
    };


    GLfloat texture[] =
    {
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
    };
    scop->background.VAO = initVertexArray((t_array){vertices, 4}, (t_array){indices, 6});
    glBindVertexArray(scop->background.VAO);
    GLuint textbuffer;
    glGenBuffers(1, &textbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (!(scop->background.programShader = initShaders("shaders/backgroundVS", "shaders/backgroundFS", scop->path)) ||
        !(scop->background.textureID = textureInit(BACKGROUND_IMAGE, scop->path)))
        return (0);
    return (1);
}

int main(int ac, char **av) {
    t_scop  scop;
    char    *objectFile;

    bzero(&scop, sizeof(t_scop));
    scop.path = strrchr(av[0], '/');
    scop.path[1] = 0;
    scop.path = av[0];
    if (!(objectFile = getObjectFile(ac, av)) || !getObjectData(&scop.object.mesh, objectFile)) {
        printUsage();
        return (-1);
    }
    if (!initWindow(&scop) ||
        !(scop.object.programShader = initShaders("shaders/vertexShader", "shaders/fragmentShader", scop.path)) ||
        !initBackground(&scop))
        return (-1);
    scop.object.VAO = initVertexArray((t_array){scop.object.mesh.vertices, scop.object.mesh.nbVertices},
                    (t_array){scop.object.mesh.Indices, scop.object.mesh.nbIndices});
    free(scop.object.mesh.vertices);
    free(scop.object.mesh.Indices);
    perspective(45.0f, (float)(WINDOW_WIDTH/WINDOW_HEIGHT), 0.1f, 100.0f, &scop.projection);
    mainLoop(&scop);
    glDeleteTextures(1, &scop.background.textureID);
    return (0);
}