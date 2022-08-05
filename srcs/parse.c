#include "scop.h" 

int getObjectData(t_object *object, char *fileName) {
    FILE *file;
    char buffer[256];
    int  indices[4];
    t_vertex    vertex;
    int ret;

    ret = 0;
    if (!(file = fopen(fileName, "r")))
        return (0);
    bzero(object, sizeof(t_object));
    while (1) {
        ret = fscanf(file, "%s", buffer);
        if (ret == EOF) {
           /* printf("mtllib = %s\n", object->mtllib);
            for (int n = 0; n < object->nbVertices; n++)
                printf("x = %f, y = %f, z = %f\n", object->vertices[n].x, object->vertices[n].y, object->vertices[n].z);
            for (int n = 0; n < object->nbTriangleIndices; n++)
                printf("%d,  %d, %d\n", object->triangleIndices[n * 3], object->triangleIndices[n * 3 + 1], object->triangleIndices[n * 3 + 2]);
            for (int n = 0; n < object->nbSquareIndices; n++)
            printf("%d,  %d, %d, %d\n", object->squareIndices[n * 4], object->squareIndices[n * 4 + 1], object->squareIndices[n * 4 + 2], object->squareIndices[n * 4 + 3]);
           */ return (1);
        }
        if (!strcmp(buffer, "v")) {
            object->vertices = realloc(object->vertices, sizeof(t_vertex) * (object->nbVertices + 1));
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            object->vertices[object->nbVertices++] = vertex;
        }
        else if (!strcmp(buffer, "f")) {
            ret = fscanf(file, "%d %d %d %d\n", &indices[0], &indices[1], &indices[2], &indices[3]);
            if (ret == 3) {
                object->triangleIndices = realloc(object->triangleIndices, sizeof(GLuint) * (object->nbTriangleIndices + 1) * 3);
                memcpy(&object->triangleIndices[object->nbTriangleIndices++ * 3], indices, sizeof(GLuint) * 3);
            }
            else {
                object->squareIndices = realloc(object->squareIndices, sizeof(GLuint) * (object->nbSquareIndices + 1) * 4);
                memcpy(&object->squareIndices[object->nbSquareIndices++ * 4], indices, sizeof(GLuint) * 4);
            }
        }
        else if (!strcmp(buffer, "mtllib")) {
            if (fscanf(file, "%s \n", buffer))
                object->mtllib = strdup(buffer);
        }
        else
            fscanf(file, "\n");
    }

    return (1);
}