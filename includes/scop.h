#ifndef SCOP_H
# define SCOP_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

# define   PI           3.14159265358979323846
# define WINDOW_WIDTH   1700
# define WINDOW_HEIGHT  1050

# define MAX_FPS        60
# define LOADED         1
# define SKYBOX_TEXTURE(x)  x & 7
# define TEX_PER_SEGMENT 5
# define TEX_OBJECT_VALUE 10

//# define FACE_CULLING

# define CUBE_MAP_FILE "texture/newport_loft.hdr"//"texture/cave.hdr"//"texture/panorama_map.hdr"//"texture/cave_entry_in_the_forest.hdr"//
# define CUBE_MAP_RESOLUTION 1024
# define PEFILLER_RESOLUTION 128
# define IRRADIANCE_MAP_RESOLUTION 16

# define DEFAULT_TEXTURE "texture/bois.jpeg"


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

struct s_usemtl {
    char *name;
    size_t start;
    size_t size;
    char *texture;
};

typedef struct s_usemtl t_usemtl;

struct s_mesh {
    char        *mltFile;
    t_array     vertices;
    t_array     uvs;
    t_array     normales;
    t_array     indices;
    t_usemtl    *segments;
    size_t      segmentNb;
    t_vertex    min;
    t_vertex    max;
};

typedef struct s_mesh t_mesh;

struct s_texture {
    void   *data;       
    int             x;
    int             y;
    int             numColCh;
};

typedef struct s_texture t_texture;

struct s_segment {
    GLuint      textureID;
    GLuint      normalTextureID;
    GLuint      metalTextureID;
    GLuint      routhTextureID;
    GLuint      AOTextureID;
    GLuint      VAO;
};

typedef struct s_segment t_segment;

struct s_object {
    t_segment   *segments;
    GLuint      segmentNb;
    t_mesh      mesh;
    t_vertex    rotation;
    t_vertex    position;
    GLuint      programShader;
};

typedef struct s_object t_object;

struct s_background {
    GLuint  VAO;
    GLuint  textureID;
    GLuint  programShader;
};

typedef struct s_background t_background;

struct s_cubeMapTextures {
    GLuint  evironementID;
    GLuint  irradianceID;
    GLuint  prefillerID;
    GLuint  brdfID;
};

typedef struct s_cubeMapTextures t_cubeMapTextures;

struct s_option {
    char        *texture;
    char        *background;
    char        IBL;
    char        smoothColor;
};

typedef struct s_option t_option;

struct s_textureInfo {
    t_texture   texture;
    char        status;
};

typedef struct s_textureInfo t_textureInfo;

struct s_textureLoader {
    t_textureInfo   cubeMap;
    t_textureInfo   *object;
    char            **texturesName;
    GLuint          segmentNb;
    GLuint          defaultTextureID;
    unsigned int    texturesLeft;
};

typedef struct s_textureLoader t_textureLoader;

struct s_scop {
    GLFWwindow  *window;
    char        *path;
    t_option    option;
    GLfloat     transition;
    t_position  mouse;
    t_background background;
    t_object    object;
    t_textureLoader textures;
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
void mat4SetIdentity(t_mat4 *matrix);
t_vertex mat4Vec3Mult(t_mat4 mat, t_vertex vec);
void mat4Transpose(t_mat4 *matrix);

//  vector.c

void normalise(t_vertex *vector);
t_vertex vec3sub(t_vertex v1, t_vertex v2);
t_vec2 vec2sub(t_vec2 v1, t_vec2 v2);


//  loop.c

void setModelMatrix(t_scop *scop);
void mainLoop(t_scop *scop);

//  cubeMap.c
int initBackground(t_scop *scop);
void *loadCubeMap(void *data);
t_cubeMapTextures createCubeMapFromEquirectangular(t_texture texture, char *path, GLuint cubeVAO, char IBL);

// textureLoading.c
void bindAllTextures(t_scop *scop);


//  parce_file.c

int getObjectData(t_mesh *mesh, char *fileName);

//  parce_data.c

int parseArguments(int ac, char **av, t_scop  *scop);
void parseNormals(t_scop  *scop);

//  utils.c

char *getShaderSource(char *fileName);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void printUsage();
void freeMeshData(t_mesh *mesh);
void freeAll(t_scop *scop);
void showFPS(GLFWwindow *window);

//  str_utils.c
char *ft_strjoin(char const *s1, char const *s2);
char *ft_replaceStr(char *str, char *ref, char *replace);
char *getObjectFile(int ac, char **av);
char *getOption(char *option, int ac, char **av, char *object);

//  init.c

int initWindow(t_scop *scop);
GLuint initShaders(char *vertexShaderFile, char *fragmentShaderFile, char *path);

// VAO.c
GLuint initVertexArray(t_array vertices);
void initElementArray(GLuint VAO, t_array indices);
void addArrayBuffer(GLuint VAO, t_array array, int dataSize, int arrayIndex);

//  event.c

void getEvents(t_scop *scop);
void initGetKeysEvent();

//  draw.c

void drawBackground(t_scop *scop);
void drawObject(t_scop *scop);

#endif