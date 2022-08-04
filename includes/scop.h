#ifndef SCOP_H
# define SCOP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

# define   PI           3.14159265358979323846 

#define IDENTITY_MAT4 {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

typedef GLfloat t_mat4[4][4];

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

void mat4Traslate(t_mat4 *matrice, t_vertex vector);
void perspective(float fov, float ratio, float near, float far, t_mat4 *result);
void rotate(t_mat4 matrix, float angle, t_vertex vector, t_mat4 *result);
void mat4Scale(t_mat4 *result, t_vertex vector);
void mat4Mult(t_mat4 mat1, t_mat4 mat2, t_mat4 *result);
void printMat4(t_mat4 mat4);

#endif