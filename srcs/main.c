#include "scop.h" 

GLfloat scopScaleValue = 1.0f;
GLuint  scopScaleID;

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
           /* printf("mtllib = %s\n", object->mtllib);
            for (int n = 0; n < object->nbVertices; n++)
                printf("x = %f, y = %f, z = %f\n", object->vertices[n].x, object->vertices[n].y, object->vertices[n].z);
            for (int n = 0; n < object->nbTriangleIndices; n++)
                printf("%d,  %d, %d\n", object->triangleIndices[n * 3], object->triangleIndices[n * 3 + 1], object->triangleIndices[n * 3 + 2]);
            for (int n = 0; n < object->nbSquareIndices; n++)
            printf("%d,  %d, %d, %d\n", object->squareIndices[n * 4], object->squareIndices[n * 4 + 1], object->squareIndices[n * 4 + 2], object->squareIndices[n * 4 + 3]);
           */ return (1);
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        scopScaleValue += scopScaleValue / 10;
    else if (yoffset < 0)
        scopScaleValue -= scopScaleValue / 10;
    if (yoffset)
        glUniform1f(scopScaleID, scopScaleValue);
}

int initWindow(t_scop *scop) {
    if( !glfwInit() )
        return (0);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // On veut OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux ; ne devrait pas être nécessaire
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
    if(!(scop->window = glfwCreateWindow( 400, 300, "Scop", NULL, NULL))) {
        glfwTerminate();
        return (0);
    }
    glfwMakeContextCurrent(scop->window);
//    glViewport(0, 0, 400, 300);
    if (glewInit() != GLEW_OK)
        return (0);
//    scop->monitor = glfwGetWindowMonitor(scop->window);	
    glfwSetInputMode(scop->window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetScrollCallback(scop->window, scroll_callback);
    return (1);
}

void mainLoop(t_scop *scop) {
    GLuint modelloc = glGetUniformLocation(scop->programShader, "model");
    GLuint viewloc = glGetUniformLocation(scop->programShader, "view");
    GLuint projloc = glGetUniformLocation(scop->programShader, "proj");

    float rotation = 0;
    double prevTime = glfwGetTime();

    glBindVertexArray(scop->VAO);
    glUseProgram(scop->programShader);
    glUniform1f(scopScaleID, scopScaleValue);
    glEnable(GL_DEPTH_TEST);

    while ( glfwGetKey(scop->window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(scop->window) == 0 ) {


        double crtTime = glfwGetTime();
        if (crtTime - prevTime >=  1 / 60) {
            prevTime = crtTime;
            rotation += 0.3f;
        }

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        t_mat4 tmp = IDENTITY_MAT4;
        t_mat4 res = IDENTITY_MAT4;

        t_mat4 model = IDENTITY_MAT4;
        t_mat4 view = IDENTITY_MAT4;
        t_mat4 proj = IDENTITY_MAT4;

        rotate(tmp, rotation, (t_vertex){0.0f, 1.0f, 0.0f}, &model);
       /* rotate(model, rotation, (t_vertex){1.0f, 0.0f, 0.0f}, &tmp);
        memcpy(&model, &tmp, sizeof(t_mat4));
        rotate(model, rotation, (t_vertex){0.0f, 0.0f, 1.0f}, &tmp);
        memcpy(&model, &tmp, sizeof(t_mat4));*/
        mat4Traslate(&view, (t_vertex){0.0f, -0.5f, -2.0f});
       // printMat4(view);
     /*   mat4Scale(&tmp, (t_vertex){0.2f, 0.2f, 0.2f});
        mat4Mult(tmp, model, &res);
        memcpy(&model, &res, sizeof(t_mat4));*/
       // mat4Traslate(&model, (t_vertex){0.0f, -10.0f, 0.0f});
        perspective(45.0f, (float)(400.0f/300.0f), 0.1f, 100.0f, &proj);
       // printMat4(proj);
      /*  mat4Mult(view, proj, &res);
        memcpy(&view, &res, sizeof(t_mat4));
        mat4Mult(model, view, &res);
        memcpy(&model, &res, sizeof(t_mat4));*/
 


        glUniformMatrix4fv(modelloc, 1, GL_FALSE, (GLfloat*)model);
        glUniformMatrix4fv(viewloc, 1, GL_FALSE, (GLfloat*)view);
        glUniformMatrix4fv(projloc, 1, GL_FALSE, (GLfloat*)proj);
		//glDrawElements(GL_TRIANGLES, scop->object.nbTriangleIndices * 3, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(scop->window);
        glfwPollEvents();
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
    const char* vertexShaderSource = getShaderSource("shaders/vertexShader");
    const char* fragmentShaderSource = getShaderSource("shaders/fragmentShader");
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
    scopScaleID = glGetUniformLocation(scop.programShader, "scale");
    mainLoop(&scop);
    return (0);
}