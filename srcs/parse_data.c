#include "scop.h" 

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
     //   if (!scop->object.mesh.segments[0].texture)
            scop->option.texture = DEFAULT_TEXTURE;
    }
    else
        scop->option.texture = scop->option.texture;
    return (1);
}

void getTanAndBiTan(t_scop  *scop) {
    t_array  segment;
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
    for (int n = 0; n < scop->object.segmentNb; n++) {
        segment.data = tangent + scop->object.mesh.segments[n].start;
        segment.size = scop->object.mesh.segments[n].size;
        addArrayBuffer(scop->object.segments[n].VAO, segment, sizeof(t_vertex), 3);
        segment.data = bitangent + scop->object.mesh.segments[n].start;
        addArrayBuffer(scop->object.segments[n].VAO, segment, sizeof(t_vertex), 4);
    }
    free(tangent);
    free(bitangent);
}

void parseNormals(t_scop  *scop) {
    t_vertex normal;
    t_vertex *normals;
    t_array  segment;

 /*   normals = scop->object.mesh.normales.data;
    for (int n = 0; n < scop->object.mesh.normales.size; n += 3) {
        normal.x = normals[n].x + normals[n + 1].x + normals[n + 2].x;
        normal.y = normals[n].y + normals[n + 1].y + normals[n + 2].y;
        normal.z = normals[n].z + normals[n + 1].z + normals[n + 2].z;
        normalise(&normal);
        normals[n] = normal;
        normals[n + 1] = normal;
        normals[n + 2] = normal;
    }*/
    for (int n = 0; n < scop->object.segmentNb; n++) {
        segment.data = scop->object.mesh.normales.data + scop->object.mesh.segments[n].start * sizeof(t_vertex);
        segment.size = scop->object.mesh.segments[n].size;
        addArrayBuffer(scop->object.segments[n].VAO, segment, sizeof(t_vertex), 2);
    }
    getTanAndBiTan(scop);
}