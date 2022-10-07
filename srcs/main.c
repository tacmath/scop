#include "scop.h" 

float ObjectSize = 0;

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

    scop->background.VAO = initVertexArray((t_array){vertices, 4});
    initElementArray(scop->background.VAO, (t_array){indices, 6});
    addArrayBuffer(scop->background.VAO, (t_array){texture, 8}, 2 * sizeof(GLfloat), 1);

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
    if (!(scop->option.texture = getOption("-t", ac, av, objectFile))) {
        if (scop->object.mesh.textureFile)                                      // a changer option.texture fait pas trop de sens
            scop->option.texture = scop->object.mesh.textureFile;
        else
            scop->option.texture = strdup(DEFAULT_TEXTURE);
    }
    else
        scop->option.texture = strdup(scop->option.texture);
    return (1);
}

int main(int ac, char **av) {
    t_scop  scop;

    bzero(&scop, sizeof(t_scop));
    if (!parseArguments(ac, av, &scop) ||
        !initWindow(&scop) ||
        !initBackground(&scop) ||
        !(scop.object.textureID = textureInit(scop.option.texture, scop.path)))
        return (-1);
    free(scop.option.texture);
    scop.object.VAO = initVertexArray(scop.object.mesh.vertices);
    if (scop.object.mesh.indices.size) {
        if (!(scop.object.programShader = initShaders("shaders/vertexShader", "shaders/fragmentShader", scop.path)))
            return (-1);
        initElementArray(scop.object.VAO, scop.object.mesh.indices);
    }
    else {
        if (!(scop.object.programShader = initShaders("shaders/completeVS", "shaders/completeFS", scop.path)))
            return (-1);
        addArrayBuffer(scop.object.VAO, scop.object.mesh.uvs, sizeof(t_vec2), 1);
        addArrayBuffer(scop.object.VAO, scop.object.mesh.normales, sizeof(t_vertex), 2);
    }
    free(scop.object.mesh.vertices.data);
    free(scop.object.mesh.indices.data);
    free(scop.object.mesh.normales.data);
    free(scop.object.mesh.uvs.data); 
    perspective(45.0f, (float)(WINDOW_WIDTH/WINDOW_HEIGHT), 0.1f, 1000.0f, &scop.projection);
    ObjectSize = scop.object.mesh.max.y - scop.object.mesh.min.y;
    mainLoop(&scop);
    glDeleteTextures(1, &scop.background.textureID);
    glDeleteTextures(1, &scop.object.textureID);
    return (0);
}