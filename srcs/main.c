#include "scop.h" 

float ObjectSize = 0;

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

void enableOptionalTextures(t_scop *scop) {
    glUseProgram(scop->object.programShader);
    for (int n = 0; n < scop->object.segmentNb; n++) {
        if (scop->object.segments[n].normalTextureID) {
            glUniform1i(glGetUniformLocation(scop->object.programShader, "hasNormalMap"), 1);
            if (scop->object.segments[n].metalTextureID && scop->object.segments[n].routhTextureID)
                glUniform1i(glGetUniformLocation(scop->object.programShader, "activatePBR"), 1);
            break ;
        }
    }
    glUseProgram(0);
}

int loadAllTextures(t_scop *scop) {
    char *fileName;
    char *defaultTexture;

    defaultTexture = ft_strjoin(scop->path, scop->option.texture);
    for (int n = 0; n < scop->object.segmentNb; n++) {
        if (!scop->object.mesh.segments[n].texture ||
            !(scop->object.segments[n].textureID = textureInit(scop->object.mesh.segments[n].texture))) {
            if (!(scop->object.segments[n].textureID = textureInit(defaultTexture)))
                return (0);
        }
        else {
            fileName = ft_replaceStr(scop->object.mesh.segments[n].texture, "Base_color", "Normal_OpenGL");
            if (!fileName)
                continue;
            scop->object.segments[n].normalTextureID = textureInit(fileName);
            free(fileName);
            fileName = ft_replaceStr(scop->object.mesh.segments[n].texture, "Base_color", "Metallic");
            if (!fileName)
                continue;
            scop->object.segments[n].metalTextureID = textureInit(fileName);
            free(fileName);
            fileName = ft_replaceStr(scop->object.mesh.segments[n].texture, "Base_color", "Roughness");
            if (!fileName)
                continue;
            scop->object.segments[n].routhTextureID = textureInit(fileName);
            free(fileName);
            fileName = ft_replaceStr(scop->object.mesh.segments[n].texture, "Base_color", "Mixed_AO");
            if (!fileName)
                continue;
            scop->object.segments[n].AOTextureID = textureInit(fileName);
            free(fileName);
        }
    }
    free(defaultTexture);
    enableOptionalTextures(scop);
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
    freeAll(&scop);
    return (0);
}