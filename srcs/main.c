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

GLfloat color[] =
{ //               COORDINATES                  /     COLORS           //
	0.8f, 0.3f,  0.02f, // Lower left corner
	0.8f, 0.3f,  0.02f, // Lower right corner
	1.0f, 0.6f,  0.32f, // Upper corner
	0.9f, 0.45f, 0.17f, // Inner left
	0.9f, 0.45f, 0.17f, // Inner right
};

    glGenVertexArrays(1, &scop->VAO);
    glBindVertexArray(scop->VAO);

    glGenBuffers(1, &scop->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, scop->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 //   glBufferData(GL_ARRAY_BUFFER, scop->object.nbVertices * sizeof(t_vertex), scop->object.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &scop->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scop->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 //   glBufferData(GL_ELEMENT_ARRAY_BUFFER, scop->object.nbTriangleIndices * 3 * sizeof(GLuint), scop->object.triangleIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int main(int ac, char **av) {
    t_scop scop;

    if (ac > 1)
        getObjectData(&scop.object, av[1]);

    if (!initWindow(&scop) || !initShaders(&scop))
        return (-1);
    initVertex(&scop);
    mainLoop(&scop);
    return (0);
}