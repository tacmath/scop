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
# define WINDOW_WIDTH   800.0
# define WINDOW_HEIGHT  600.0

#define IDENTITY_MAT4 {{1.0f,0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f,0.0f},{0.0f,0.0f,1.0f,0.0f},{0.0f,0.0f,0.0f,1.0f}};

typedef GLfloat t_mat4[4][4];

struct s_vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

typedef struct s_vertex t_vertex;

struct s_position {
    double x;
    double y;
};

typedef struct s_position t_position;

struct s_mesh {
    t_vertex    *vertices;
    GLuint      nbVertices;
    GLuint      *Indices;
    GLuint      nbIndices;
    t_vertex    min;
    t_vertex    max;
};

typedef struct s_mesh t_mesh;

struct s_texture {
    unsigned char   *data;       
    int             x;
    int             y;
    int             numColCh;
};

typedef struct s_texture t_texture;

struct s_array {
    void    *data;
    size_t  size;
};

typedef struct s_array t_array;

struct s_object {
    GLuint      VAO;
    t_mesh      mesh;
    t_vertex    rotation;
    GLuint      textureID;
    GLuint      programShader;
};

typedef struct s_object t_object;


struct s_scop {
    GLFWwindow  *window;
    GLuint      textureID;
    GLuint      programShader;
    t_position  mouse;
    t_object    background;
    t_object    object;
    t_mat4      projection;
};

typedef struct s_scop t_scop;

//  matrix.c

void mat4Traslate(t_mat4 *matrice, t_vertex vector);
void perspective(float fov, float ratio, float near, float far, t_mat4 *result);
void rotate(t_mat4 matrix, float angle, t_vertex vector, t_mat4 *result);
void mat4Scale(t_mat4 *result, t_vertex vector);
void mat4Mult(t_mat4 mat1, t_mat4 mat2, t_mat4 *result);
void printMat4(t_mat4 mat4);

//  loop.c

void mainLoop(t_scop *scop);

//  parce.c

int getObjectData(t_mesh *mesh, char *fileName);

//  utils.c

char *getShaderSource(char *fileName);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//  init.c

int initWindow(t_scop *scop);
GLuint initShaders(char *vertexShaderFile, char *fragmentShaderFile);
GLuint initVertexArray(t_array vertices, t_array indices);
int textureInit(t_scop *scop, char *fileName);

#endif