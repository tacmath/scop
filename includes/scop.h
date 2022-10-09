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
# define WINDOW_WIDTH   400.0
# define WINDOW_HEIGHT  300.0
# define MAX_FPS        60
# define BACKGROUND_IMAGE "texture/plaines.jpeg"
# define DEFAULT_TEXTURE "texture/test.jpeg"

#define IDENTITY_MAT4 {{1.0f,0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f,0.0f},{0.0f,0.0f,1.0f,0.0f},{0.0f,0.0f,0.0f,1.0f}};

typedef GLfloat t_mat4[4][4];

struct s_vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

typedef struct s_vertex t_vertex;

struct s_vec2 {
    GLfloat x;
    GLfloat y;
};

typedef struct s_vec2 t_vec2;

struct s_position {
    GLfloat x;
    GLfloat y;
};

typedef struct s_position t_position;

struct s_array {
    void    *data;
    size_t  size;
};

typedef struct s_array t_array;

struct s_indice {
    GLuint vertex;
    GLuint uvs;
    GLuint normale;
};

typedef struct s_indice t_indice;

struct s_mesh {
    char        *mltFile;
    char        *textureFile;
    t_array     vertices;
    t_array     uvs;
    t_array     normales;
    t_array     indices;
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

struct s_object {
    GLuint      VAO;
    t_mesh      mesh;
    t_vertex    rotation;
    GLuint      textureID;
    GLuint      programShader;
};

typedef struct s_object t_object;

struct s_option {
    char        *texture;
};

typedef struct s_option t_option;

struct s_scop {
    GLFWwindow  *window;
    char        *path;
    t_option    option;
    GLfloat     transition;
    t_position  mouse;
    t_object    background;
    t_object    object;
    t_mat4      projection;
    t_mat4      rotation;
    t_vertex    lightPos;
};

typedef struct s_scop t_scop;

//  matrix.c

void mat4Traslate(t_mat4 *matrice, t_vertex vector);
void perspective(float fov, float ratio, float near, float far, t_mat4 *result);
void rotate(t_mat4 matrix, float angle, t_vertex vector, t_mat4 *result);
void mat4Scale(t_mat4 *result, t_vertex vector);
void mat4Mult(t_mat4 mat1, t_mat4 mat2, t_mat4 *result);
void printMat4(t_mat4 mat4);
void normalise(t_vertex *vector);
void mat4SetIdentity(t_mat4 *matrix);

//  loop.c

void setMatrix(t_scop *scop, GLuint matrixLoc, GLuint modelMatrixLoc);
void mainLoop(t_scop *scop);

//  parce.c

int getObjectData(t_mesh *mesh, char *fileName);

//  utils.c

char *getShaderSource(char *fileName);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void printUsage();
char *getObjectFile(int ac, char **av);
char *ft_strjoin(char const *s1, char const *s2);
char *getOption(char *option, int ac, char **av, char *object);

//  init.c

int initWindow(t_scop *scop);
GLuint initShaders(char *vertexShaderFile, char *fragmentShaderFile, char *path);
GLuint textureInit(char *fileName, char *path);

// VAO.c
GLuint initVertexArray(t_array vertices);
void initElementArray(GLuint VAO, t_array indices);
void addArrayBuffer(GLuint VAO, t_array array, int dataSize, int arrayIndex);

//  event.c

void getEvents(t_scop *scop);

//  draw.c

void initUniforms(t_object *object);
void drawBackground(t_scop *scop);
void drawObject(t_scop *scop, GLuint matrixLoc, GLuint modelMatrixLoc);

#endif