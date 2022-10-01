#include "scop.h" 

void getMeshBorders(t_mesh *mesh) {
    if (!mesh->nbVertices)
        return;
    mesh->max = mesh->vertices[0];
    mesh->min = mesh->vertices[0];
    for (int n = 1; n < mesh->nbVertices; n++) {
        if (mesh->vertices[n].x > mesh->max.x)
            mesh->max.x = mesh->vertices[n].x;
        if (mesh->vertices[n].y > mesh->max.y)
            mesh->max.y = mesh->vertices[n].y;
        if (mesh->vertices[n].z > mesh->max.z)
            mesh->max.z = mesh->vertices[n].z;
        if (mesh->vertices[n].x < mesh->min.x)
            mesh->min.x = mesh->vertices[n].x;
        if (mesh->vertices[n].y < mesh->min.y)
            mesh->min.y = mesh->vertices[n].y;
        if (mesh->vertices[n].z < mesh->min.z)
            mesh->min.z = mesh->vertices[n].z;
    }
}

int getObjectData(t_mesh *mesh, char *fileName) {
    FILE *file;
    char buffer[256];
    GLuint  indices[4];
    t_vertex    vertex;
    int ret;

    ret = 0;
    if (!(file = fopen(fileName, "r")))
        return (0);
    bzero(mesh, sizeof(t_mesh));
    while (1) {
        ret = fscanf(file, "%s", buffer);
        if (ret == EOF) {
            fclose(file);
            getMeshBorders(mesh);
            for (int n = 0; n < mesh->nbIndices; n++)
                mesh->Indices[n] -= 1;
            return (1);
        }
        if (!strcmp(buffer, "v")) {
            mesh->vertices = realloc(mesh->vertices, sizeof(t_vertex) * (mesh->nbVertices + 1));
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            mesh->vertices[mesh->nbVertices++] = vertex;
        }
        else if (!strcmp(buffer, "f")) {
            ret = fscanf(file, "%d %d %d %d\n", &indices[0], &indices[1], &indices[2], &indices[3]);
            mesh->Indices = realloc(mesh->Indices, sizeof(GLuint) * (mesh->nbIndices + 3 + (3 * (ret == 4))));
            memcpy(&mesh->Indices[mesh->nbIndices], indices, sizeof(GLuint) * 3);
            mesh->nbIndices += 3;
            if (ret == 4) {
                memcpy(&mesh->Indices[mesh->nbIndices], indices, sizeof(GLuint) * 3);
                mesh->Indices[mesh->nbIndices + 1] = indices[3];
                mesh->nbIndices += 3;
            }
        }
        else
            fscanf(file, "\n");
    }
}