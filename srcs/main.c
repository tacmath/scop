#include "scop.h" 

float ObjectSize = 0;

int initBackground(t_scop *scop) {

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

    scop->background.VAO = initVertexArray((t_array){vertices, 4});
    initElementArray(scop->background.VAO, (t_array){indices, 6});
    addArrayBuffer(scop->background.VAO, (t_array){texture, 8}, 2 * sizeof(GLfloat), 1);

    if (!(scop->background.programShader = initShaders("shaders/backgroundVS", "shaders/backgroundFS", scop->path)) ||
        !(scop->background.textureID = textureInit(BACKGROUND_IMAGE, scop->path)))
        return (0);
    return (1);
}

void freeMeshData(t_mesh *mesh) {
    free(mesh->vertices.data);
    free(mesh->indices.data);
    free(mesh->normales.data);
    free(mesh->uvs.data);
    for (int n = 0; n < mesh->segmentNb; n++) {
        free(mesh->segments[n].name);
        free(mesh->segments[n].texture);
    }
    free(mesh->segments);
}

int generateVAO(t_scop  *scop) {
    t_array segment;

    scop->object.segmentNb = scop->object.mesh.segmentNb;
    if (!(scop->object.segments = calloc(sizeof(t_segment), scop->object.segmentNb)))
        return (0);
    
    if (scop->object.mesh.indices.size) {
        if (!(scop->object.programShader = initShaders("shaders/vertexShader", "shaders/fragmentShader", scop->path)))
            return (0);
        scop->object.segments[0].VAO = initVertexArray(scop->object.mesh.vertices);
        initElementArray(scop->object.segments[0].VAO, scop->object.mesh.indices);
        scop->object.segmentNb = 1;
        return (1);
    }
    if (!(scop->object.programShader = initShaders("shaders/completeVS", "shaders/completeFS", scop->path)))
        return (0);
    for (int n = 0; n < scop->object.segmentNb; n++) {
        segment.data = scop->object.mesh.vertices.data + scop->object.mesh.segments[n].start * sizeof(t_vertex);
        segment.size = scop->object.mesh.segments[n].size;
        scop->object.segments[n].VAO = initVertexArray(segment);
        segment.data = scop->object.mesh.uvs.data + scop->object.mesh.segments[n].start * sizeof(t_vec2);
        addArrayBuffer(scop->object.segments[n].VAO, segment, sizeof(t_vec2), 1);
    }
    parseNormals(scop);
    return (1);
}

char *ft_replaceStr(char *str, char *ref, char *replace) {
    char *new;
    char *tmp;
    int n;

    if (!(tmp = strstr(str, ref)) ||
        !(new = malloc(strlen(str) - strlen(ref) + strlen(replace) + 1)))
        return (0);
    n = tmp - str;
    memcpy(new, str, n);
    strcpy(new + n, replace);
    strcpy(new + n + strlen(replace), tmp + strlen(ref));
    return (new);
}

int loadAllTextures(t_scop *scop) {
    char *normalFileName;

    for (int n = 0; n < scop->object.segmentNb; n++) {
        if (!scop->object.mesh.segments[n].texture ||
            !(scop->object.segments[n].textureID = textureInit(scop->object.mesh.segments[n].texture, scop->path))) {
            if (!(scop->object.segments[n].textureID = textureInit(scop->option.texture, scop->path)))
                return (0);
        }
        else {
            normalFileName = ft_replaceStr(scop->object.mesh.segments[n].texture, "Base_color", "Normal_OpenGL");
            if (!normalFileName) {
                continue;
            }
            scop->object.segments[n].normalTextureID = textureInit(normalFileName, scop->path);
            free(normalFileName);
        }
    }
    return (1);
}

int main(int ac, char **av) {
    t_scop  scop;

    bzero(&scop, sizeof(t_scop));
    if (!parseArguments(ac, av, &scop) ||
        !initWindow(&scop) ||
        !initBackground(&scop) ||
        !generateVAO(&scop)||
        !loadAllTextures(&scop))
        return (-1);
    freeMeshData(&scop.object.mesh);
    perspective(45.0f, (float)(WINDOW_WIDTH/WINDOW_HEIGHT), 0.1f, 1000.0f, &scop.projection);
    mat4SetIdentity(&scop.rotation);
    ObjectSize = scop.object.mesh.max.y - scop.object.mesh.min.y;
    scop.lightPos = (t_vertex){scop.object.mesh.min.x * 1.5, ((scop.object.mesh.max.y - scop.object.mesh.min.y) / 2) * 1.5 - scop.object.mesh.min.y, (scop.object.mesh.max.z - scop.object.mesh.min.z) / 2 - scop.object.mesh.min.z};
    mainLoop(&scop);
    glDeleteTextures(1, &scop.background.textureID);
    for (int n = 0; n < scop.object.segmentNb; n++)
        glDeleteTextures(1, &scop.object.segments[n].textureID);
    free(scop.object.segments);
    return (0);
}