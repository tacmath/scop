#ifndef SCOP_H
# define SCOP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct s_vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

typedef struct s_vertex t_vertex;

struct s_object {
    char        *mtllib;
    t_vertex    *vertices;
    GLuint      nbVertices;
    GLuint      *squareIndices;
    GLuint      nbSquareIndices;
    GLuint      *triangleIndices;
    GLuint      nbTriangleIndices;
};

typedef struct s_object t_object;

struct s_scop {
    GLFWwindow  *window;
    GLuint      VAO;
    GLuint      VBO;
    GLuint      EBO;
    GLuint      programShader;
    t_object    object;
};

typedef struct s_scop t_scop;

#endif