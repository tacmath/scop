#include "scop.h" 

void getMeshBorders(t_mesh *mesh) {
    t_vertex *vertices;

    vertices = mesh->vertices.data;
    if (!vertices)
        return;
    mesh->max = vertices[0];
    mesh->min = vertices[0];
    for (int n = 1; n < mesh->vertices.size; n++) {
        if (vertices[n].x > mesh->max.x)
            mesh->max.x = vertices[n].x;
        if (vertices[n].y > mesh->max.y)
            mesh->max.y = vertices[n].y;
        if (vertices[n].z > mesh->max.z)
            mesh->max.z = vertices[n].z;
        if (vertices[n].x < mesh->min.x)
            mesh->min.x = vertices[n].x;
        if (vertices[n].y < mesh->min.y)
            mesh->min.y = vertices[n].y;
        if (vertices[n].z < mesh->min.z)
            mesh->min.z = vertices[n].z;
    }
}

void getIndices(FILE *file, t_mesh *mesh) {
    GLuint  indices[4];
    int ret;

    ret = fscanf(file, "%d %d %d %d\n", &indices[0], &indices[1], &indices[2], &indices[3]);
    mesh->indices.data = realloc(mesh->indices.data, sizeof(GLuint) * (mesh->indices.size + 3 + (3 * (ret == 4))));
    memcpy(&((GLuint*)mesh->indices.data)[mesh->indices.size], indices, sizeof(GLuint) * 3);
    mesh->indices.size += 3;
    if (ret == 4) {
        memcpy(&((GLuint*)mesh->indices.data)[mesh->indices.size], indices, sizeof(GLuint) * 3);
        ((GLuint*)mesh->indices.data)[mesh->indices.size + 1] = indices[3];
        mesh->indices.size += 3;
    }
}

void getVertices(FILE *file, t_array *vertices) {
    t_vertex    vertex;

    vertices->data = realloc(vertices->data, sizeof(t_vertex) * (vertices->size + 1));
    fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
    ((t_vertex*)vertices->data)[vertices->size++] = vertex;
}

void getUvs(FILE *file, t_mesh *mesh) {
    t_vec2  uv;

    mesh->uvs.data = realloc(mesh->uvs.data, sizeof(t_vec2) * (mesh->uvs.size + 1));
    fscanf(file, "%f %f\n", &uv.x, &uv.y);
    ((t_vec2*)mesh->uvs.data)[mesh->uvs.size++] = uv;
}

void printObjectData(t_mesh *mesh) {
    GLuint *indices = mesh->indices.data;
    t_vertex *vertices = mesh->vertices.data;
    t_vertex *normales = mesh->normales.data;
    t_vec2  *uvs =mesh->uvs.data;

    printf("Vertices :\n\n");
    for (int n = 0; n < mesh->vertices.size; n++)
        printf("v = (%7.4f, %7.4f, %7.4f)\n", vertices[n].x, vertices[n].y, vertices[n].z);
    printf("\nUvs :\n\n");
    for (int n = 0; n < mesh->uvs.size; n++)
        printf("uv = %7.4f, %7.4f\n", uvs[n].x, uvs[n].y);
    printf("\nNormales :\n\n");
    for (int n = 0; n < mesh->normales.size; n++)
        printf("n = (%7.4f, %7.4f, %7.4f)\n", normales[n].x, normales[n].y, normales[n].z);
    printf("\nIndices :\n\n");
    for (int n = 0; n < (mesh->indices.size / 3); n++)
        printf("indices = %6d, %6d, %6d\n", indices[n * 3], indices[n * 3 + 1], indices[n * 3 + 2]);
    printf("\nmin = (%7.4f, %7.4f, %7.4f)\n", mesh->min.x, mesh->min.y, mesh->min.z);
    printf("max = (%7.4f, %7.4f, %7.4f)\n", mesh->max.x, mesh->max.y, mesh->max.z);
}

int getObjectData(t_mesh *mesh, char *fileName) {
    FILE *file;
    char buffer[256];
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
            for (int n = 0; n < mesh->indices.size; n++)
                ((GLuint*)mesh->indices.data)[n] -= 1;
//            printObjectData(mesh);
            return (1);
        }
        if (!strcmp(buffer, "v"))
            getVertices(file, &mesh->vertices);
        else if (!strcmp(buffer, "vt"))
            getUvs(file, mesh);
        else if (!strcmp(buffer, "vn"))
            getVertices(file, &mesh->normales);
        else if (!strcmp(buffer, "f"))
            getIndices(file, mesh);
        else
            fscanf(file, "\n");
    }
}