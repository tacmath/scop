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

int parseArguments(int ac, char **av, t_scop  *scop) {
    char    *objectFile;
    
    scop->path = strrchr(av[0], '/');
    scop->path[1] = 0;
    scop->path = av[0];
    if (!(objectFile = getObjectFile(ac, av)) ||
        !getObjectData(&scop->object.mesh, objectFile)) {
        printUsage();
        return (0);
    }
    if (!(scop->option.texture = getOption("-t", ac, av, objectFile))) {
        if (scop->object.mesh.textureFile)                                      // a changer option.texture fait pas trop de sens
            scop->option.texture = scop->object.mesh.textureFile;
        else
            scop->option.texture = strdup(DEFAULT_TEXTURE);
    }
    else
        scop->option.texture = strdup(scop->option.texture);
    return (1);
}

void getTanAndBiTan(t_scop  *scop) {
    t_vertex    vec3;
    t_vertex    *tangent;
    t_vertex    *bitangent;
    t_vertex    *pos;
    t_vec2      *uv;
    t_vertex    edge1;
    t_vertex    edge2;
    t_vec2      deltaUV1;
    t_vec2      deltaUV2;
    GLfloat     f;

    pos = scop->object.mesh.vertices.data;
    uv = scop->object.mesh.uvs.data;
    if (!(tangent = malloc(sizeof(t_vertex) * scop->object.mesh.vertices.size)) ||
        !(bitangent = malloc(sizeof(t_vertex) * scop->object.mesh.vertices.size)))
    return ;
    for (int n = 0; n < scop->object.mesh.vertices.size; n += 3) {
        edge1.x = pos[n + 1].x - pos[n].x;
        edge1.y = pos[n + 1].y - pos[n].y;
        edge1.z = pos[n + 1].z - pos[n].z;
        edge2.x = pos[n + 2].x - pos[n].x;
        edge2.y = pos[n + 2].y - pos[n].y;
        edge2.z = pos[n + 2].z - pos[n].z;
        deltaUV1.x = uv[n + 1].x - uv[n].x;
        deltaUV1.y = uv[n + 1].y - uv[n].y;
        deltaUV2.x = uv[n + 2].x - uv[n].x;
        deltaUV2.y = uv[n + 2].y - uv[n].y;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        vec3.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        vec3.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        vec3.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        normalise(&vec3);
        tangent[n] = vec3;
        tangent[n + 1] = vec3;
        tangent[n + 2] = vec3;

        vec3.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        vec3.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        vec3.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        normalise(&vec3);
        bitangent[n] = vec3;
        bitangent[n + 1] = vec3;
        bitangent[n + 2] = vec3;
        
    }
    addArrayBuffer(scop->object.VAO, (t_array){tangent, scop->object.mesh.vertices.size}, sizeof(t_vertex), 3);
    addArrayBuffer(scop->object.VAO, (t_array){bitangent, scop->object.mesh.vertices.size}, sizeof(t_vertex), 4);
    free(tangent);
    free(bitangent);
}

void parseNormals(t_scop  *scop) {
    t_vertex normal;
    t_vertex *normals;

    normals = scop->object.mesh.normales.data;
    for (int n = 0; n < scop->object.mesh.normales.size; n += 3) {
        normal.x = normals[n].x + normals[n + 1].x + normals[n + 2].x;
        normal.y = normals[n].y + normals[n + 1].y + normals[n + 2].y;
        normal.z = normals[n].z + normals[n + 1].z + normals[n + 2].z;
        normalise(&normal);
        normals[n] = normal;
        normals[n + 1] = normal;
        normals[n + 2] = normal;
    }
    addArrayBuffer(scop->object.VAO, scop->object.mesh.normales, sizeof(t_vertex), 2);
    getTanAndBiTan(scop);
}

int generateVAO(t_scop  *scop) {

    scop->object.VAO = initVertexArray(scop->object.mesh.vertices);
    if (scop->object.mesh.indices.size) {
        if (!(scop->object.programShader = initShaders("shaders/vertexShader", "shaders/fragmentShader", scop->path)))
            return (0);
        initElementArray(scop->object.VAO, scop->object.mesh.indices);
    }
    else {
        if (!(scop->object.programShader = initShaders("shaders/completeVS", "shaders/completeFS", scop->path)))
            return (0);
        addArrayBuffer(scop->object.VAO, scop->object.mesh.uvs, sizeof(t_vec2), 1);
        parseNormals(scop);  
    }
    free(scop->object.mesh.vertices.data);
    free(scop->object.mesh.indices.data);
    free(scop->object.mesh.normales.data);
    free(scop->object.mesh.uvs.data); 
    return (1);
}

int main(int ac, char **av) {
    t_scop  scop;

    bzero(&scop, sizeof(t_scop));
    if (!parseArguments(ac, av, &scop) ||
        !initWindow(&scop) ||
        !initBackground(&scop) ||
        !(scop.object.textureID = textureInit(scop.option.texture, scop.path)))
        return (-1);
    free(scop.option.texture);
    if (!generateVAO(&scop))
        return (-1);
    perspective(45.0f, (float)(WINDOW_WIDTH/WINDOW_HEIGHT), 0.1f, 1000.0f, &scop.projection);
    mat4SetIdentity(&scop.rotation);
    ObjectSize = scop.object.mesh.max.y - scop.object.mesh.min.y;
    scop.lightPos = (t_vertex){scop.object.mesh.min.x * 1.5, ((scop.object.mesh.max.y - scop.object.mesh.min.y) / 2) * 1.5 - scop.object.mesh.min.y, (scop.object.mesh.max.z - scop.object.mesh.min.z) / 2 - scop.object.mesh.min.z};
    mainLoop(&scop);
    glDeleteTextures(1, &scop.background.textureID);
    glDeleteTextures(1, &scop.object.textureID);
    return (0);
}