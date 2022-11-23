#include "scop.h" 

float ObjectSize = 0;

int loadAllTextures(t_scop *scop);

int generateRandomFaceColor(GLuint size, GLuint VAO) {
    t_vertex *colors;
    t_vertex color;

    if (!(colors = malloc(sizeof(t_vertex) * size)))
        return (0);
    for (int n = 0; n < size / 3; n++) {
        color.x = ((float)rand()/(float)(RAND_MAX));
        color.y = ((float)rand()/(float)(RAND_MAX));
        color.z = ((float)rand()/(float)(RAND_MAX));
        colors[n * 3] = color;
        colors[n * 3 + 1] = color;
        colors[n * 3 + 2] = color;
    }
    addArrayBuffer(VAO, (t_array){colors, size}, sizeof(t_vertex), 1);
    free(colors);
    return (1);
}

int generateVAO(t_scop  *scop) {
    t_array segment;

    scop->object.segmentNb = scop->object.mesh.segmentNb;
    if (!(scop->object.segments = calloc(sizeof(t_segment), scop->object.segmentNb)))
        return (0);
    
    if (!scop->object.mesh.normales.size || !scop->object.mesh.uvs.size) {
        if (!(scop->object.programShader = initShaders((scop->option.smoothColor) ? "shaders/VertexShader.glsl" : "shaders/faceColorVS.glsl", "shaders/fragmentShader.glsl", scop->path)))
            return (0);
        scop->object.segments[0].VAO = initVertexArray(scop->object.mesh.vertices);
        if (!generateRandomFaceColor(scop->object.mesh.vertices.size, scop->object.segments[0].VAO))
            return (0);
        scop->object.segmentNb = 1;
        return (1);
    }
    scop->option.IBL = 1;
    if (!(scop->object.programShader = initShaders("shaders/completeVS.glsl", "shaders/completeFS.glsl", scop->path)))
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
    perspective(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f, &scop.projection);
    mat4SetIdentity(&scop.rotation);
    ObjectSize = scop.object.mesh.max.y - scop.object.mesh.min.y;
    scop.lightPos = (t_vertex){scop.object.mesh.min.x * 1.5, ((scop.object.mesh.max.y - scop.object.mesh.min.y) / 2) * 1.5 - scop.object.mesh.min.y, (scop.object.mesh.max.z - scop.object.mesh.min.z) / 2 - scop.object.mesh.min.z};
    initGetKeysEvent();
    mainLoop(&scop);
    freeAll(&scop);
    return (0);
}