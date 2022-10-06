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

int parseArguments(int ac, char **av, t_scop  *scop) {
    char    *objectFile;
    
    scop->path = strrchr(av[0], '/');
    scop->path[1] = 0;
    scop->path = av[0];
    if (!(objectFile = getObjectFile(ac, av)) ||
        !getObjectData(&scop->object.mesh, objectFile)) {
        printUsage();
        return (0);
    }
    if (!(scop->option.texture = getOption("-t", ac, av, objectFile)))
        scop->option.texture = DEFAULT_TEXTURE;
    return (1);
}

int main(int ac, char **av) {
    t_scop  scop;

    bzero(&scop, sizeof(t_scop));
    if (!parseArguments(ac, av, &scop) ||
        !initWindow(&scop) ||
        !initBackground(&scop))
        return (-1);
    if (!(scop.object.programShader = initShaders("shaders/vertexShader", "shaders/fragmentShader", scop.path)) ||
        !(scop.object.textureID = textureInit(scop.option.texture, scop.path)))
        return (-1);
    scop.object.VAO = initVertexArray(scop.object.mesh.vertices, scop.object.mesh.indices);
    free(scop.object.mesh.vertices.data);
    free(scop.object.mesh.indices.data);
    perspective(45.0f, (float)(WINDOW_WIDTH/WINDOW_HEIGHT), 0.1f, 1000.0f, &scop.projection);
    mainLoop(&scop);
    glDeleteTextures(1, &scop.background.textureID);
    return (0);
}