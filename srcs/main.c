#include "scop.h" 

void initVertex(t_scop *scop) {
    
// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES   
	-0.5f, 0.0f,  0.5f,
	-0.5f, 0.0f, -0.5f,
	 0.5f, 0.0f, -0.5f,
	 0.5f, 0.0f,  0.5f,
	 0.0f, 0.8f,  0.0f,
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};
// Indices for vertices order
/*
GLfloat color[] =
{ //               COORDINATES                  /     COLORS           //
	0.8f, 0.3f,  0.02f, // Lower left corner
	0.8f, 0.3f,  0.02f, // Lower right corner
	1.0f, 0.6f,  0.32f, // Upper corner
	0.9f, 0.45f, 0.17f, // Inner left
	0.9f, 0.45f, 0.17f, // Inner right
};

GLfloat texture[] =
{ //     COORDINATES   
	0.0f, 0.0f,         //bot front left
	1.0f, 0.0f,         //bot back left
	0.0f, 0.0f,         //bot back right
	1.0f, 0.0f,         //bot front right
	0.5f, 2.0f,         //top
};*/

    GLuint VBO, EBO;
    glGenVertexArrays(1, &scop->object.VAO);
    glBindVertexArray(scop->object.VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    scop->object.mesh.nbIndices = sizeof(indices);
 
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
/*
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    GLuint textbuffer;
    glGenBuffers(1, &textbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
*/
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

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

    if (ac > 1)
        getObjectData(&scop.object.mesh, av[1]);
    if (!initWindow(&scop) ||
        !(scop.programShader = initShaders("shaders/vertexShader", "shaders/fragmentShader")))
        return (-1);

    textureInit(&scop, "texture/pop_cat.png");
    if (ac > 1) {
        scop.object.VAO = initVertexArray((t_array){scop.object.mesh.vertices, scop.object.mesh.nbVertices},
                        (t_array){scop.object.mesh.Indices, scop.object.mesh.nbIndices});
    }
    else
        initVertex(&scop);
    mainLoop(&scop);
    glDeleteTextures(1, &scop.textureID);
    return (0);
}