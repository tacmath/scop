#include "scop.h" 


GLuint initVertexArray(t_array vertices, t_array indices) {
    GLuint VBO, EBO, VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size * sizeof(t_vertex), vertices.data, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size * sizeof(GLuint), indices.data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return (VAO);
}


void initBackground(t_scop *scop) {

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

    scop->background.programShader = initShaders("shaders/backgroundVS", "shaders/backgroundFS");
/*

    */
}

void textureInit(t_scop *scop, char *fileName) {
    t_texture texture;

    stbi_set_flip_vertically_on_load(1);
    texture.data = stbi_load(fileName, &texture.x, &texture.y, &texture.numColCh, 0);
    glGenTextures(1, &scop->textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scop->textureID);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.x, texture.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture.data);
    glBindTexture(GL_TEXTURE_2D, 0);
}



int main(int ac, char **av) {
    t_scop scop;

    bzero(&scop, sizeof(t_scop));
    if (ac > 1)
        getObjectData(&scop.object.mesh, av[1]);
    else
        return (-1);
    if (!initWindow(&scop) ||
        !(scop.programShader = initShaders("shaders/vertexShader", "shaders/fragmentShader")))
        return (-1);

    textureInit(&scop, "texture/pop_cat.png");
    if (ac > 1) {
        scop.object.VAO = initVertexArray((t_array){scop.object.mesh.vertices, scop.object.mesh.nbVertices},
                        (t_array){scop.object.mesh.Indices, scop.object.mesh.nbIndices});
        free(scop.object.mesh.vertices);
        free(scop.object.mesh.Indices);
    }
    perspective(45.0f, (float)(400.0f/300.0f), 0.1f, 100.0f, &scop.projection);
    initBackground(&scop);
    mainLoop(&scop);
    glDeleteTextures(1, &scop.textureID);
    return (0);
}