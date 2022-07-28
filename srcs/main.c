#include "scop.h" 


int getObjectData(t_object *object, char *fileName) {
    FILE *file;
    char buffer[256];
    int  indices[4];
    t_vertex    vertex;
    int ret;

    ret = 0;
    if (!(file = fopen(fileName, "r")))
        return (0);
    bzero(object, sizeof(t_object));
    while (1) {
        ret = fscanf(file, "%s", buffer);
        if (ret == EOF) {
          /*  printf("mtllib = %s\n", object->mtllib);
            for (int n = 0; n < object->nbVertices; n++)
                printf("x = %f, y = %f, z = %f\n", object->vertices[n].x, object->vertices[n].y, object->vertices[n].z);
            for (int n = 0; n < object->nbTriangleIndices; n++)
                printf("%d,  %d, %d\n", object->triangleIndices[n * 3], object->triangleIndices[n * 3 + 1], object->triangleIndices[n * 3 + 2]);
            for (int n = 0; n < object->nbSquareIndices; n++)
            printf("%d,  %d, %d, %d\n", object->squareIndices[n * 4], object->squareIndices[n * 4 + 1], object->squareIndices[n * 4 + 2], object->squareIndices[n * 4 + 3]);
            */return (1);
        }
        if (!strcmp(buffer, "v")) {
            object->vertices = realloc(object->vertices, sizeof(t_vertex) * (object->nbVertices + 1));
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            object->vertices[object->nbVertices++] = vertex;
        }
        else if (!strcmp(buffer, "f")) {
            ret = fscanf(file, "%d %d %d %d\n", &indices[0], &indices[1], &indices[2], &indices[3]);
            if (ret == 3) {
                object->triangleIndices = realloc(object->triangleIndices, sizeof(GLuint) * (object->nbTriangleIndices + 1) * 3);
                memcpy(&object->triangleIndices[object->nbTriangleIndices++ * 3], indices, sizeof(GLuint) * 3);
            }
            else {
                object->squareIndices = realloc(object->squareIndices, sizeof(GLuint) * (object->nbSquareIndices + 1) * 4);
                memcpy(&object->squareIndices[object->nbSquareIndices++ * 4], indices, sizeof(GLuint) * 4);
            }
        }
        else if (!strcmp(buffer, "mtllib")) {
            if (fscanf(file, "%s \n", buffer))
                object->mtllib = strdup(buffer);
        }
        else
            fscanf(file, "\n");
    }

    return (1);
}

int initWindow(t_scop *scop) {
    if( !glfwInit() )
        return (0);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // On veut OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux ; ne devrait pas être nécessaire
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
    if(!(scop->window = glfwCreateWindow( 1960, 1080, "Scop", NULL, NULL))) {
        glfwTerminate();
        return (0);
    }
    glfwMakeContextCurrent(scop->window);
    if (glewInit() != GLEW_OK)
        return (0);
//    scop->monitor = glfwGetWindowMonitor(scop->window);	
    glfwSetInputMode(scop->window, GLFW_STICKY_KEYS, GL_TRUE);
    return (1);
}

void mainLoop(t_scop *scop) {
    glBindVertexArray(scop->VAO);

    while ( glfwGetKey(scop->window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(scop->window) == 0 ) {
        glfwPollEvents();

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
		
        glUseProgram(scop->programShader);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(scop->window);
    }
}

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

int initShaders(t_scop *scop) {
    const char* vertexShaderSource = getShaderSource("/Users/mtaquet/projet/scop/shaders/vertexShader");
    const char* fragmentShaderSource = getShaderSource("/Users/mtaquet/projet/scop/shaders/fragmentShader");
   // printf("\n %s \n%s\n",vertexShaderSource, fragmentShaderSource);
    if (!vertexShaderSource || !fragmentShaderSource)
        return (0);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	scop->programShader = glCreateProgram();
	glAttachShader(scop->programShader, vertexShader);
	glAttachShader(scop->programShader, fragmentShader);
	glLinkProgram(scop->programShader);

    free((void*)vertexShaderSource);
    free((void*)fragmentShaderSource);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    return (1);
}

void initVertex(t_scop *scop) {
    static const GLfloat g_vertex_buffer_data[] = {
        -0.2f, -0.2f, 0.0f,
        0.2f, -0.2f, 0.0f,
        0.0f,  0.2f, 0.0f,
        };

    glGenVertexArrays(1, &scop->VAO);
    glBindVertexArray(scop->VAO);

    glGenBuffers(1, &scop->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, scop->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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