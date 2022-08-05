#include "scop.h" 

void printMat4(t_mat4 mat4) {
    printf("\n%4f %4f %4f %4f\n", mat4[0][0], mat4[0][1], mat4[0][2], mat4[0][3]);
    printf("%4f %4f %4f %4f\n", mat4[1][0], mat4[1][1], mat4[1][2], mat4[1][3]);
    printf("%4f %4f %4f %4f\n", mat4[2][0], mat4[2][1], mat4[2][2], mat4[2][3]);
    printf("%4f %4f %4f %4f\n", mat4[3][0], mat4[3][1], mat4[3][2], mat4[3][3]);
}

void mat4Mult(t_mat4 mat1, t_mat4 mat2, t_mat4 *result) {
    float res;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            res = 0;
            for (int n = 0; n < 4; n++)
                res += mat1[x][n] * mat2[n][y];         //(*result)[x][y] = mat1[x][0] * mat2[0][y] + mat1[x][1] * mat2[1][y] + mat1[x][2] * mat2[2][y] + mat1[x][3] * mat2[3][y];
            (*result)[x][y] = res;
        }
    }
}

void mat4Traslate(t_mat4 *matrice, t_vertex vector) {
    (*matrice)[0][3] = vector.x;
    (*matrice)[1][3] = vector.y;
    (*matrice)[2][3] = vector.z;
}

void mat4Scale(t_mat4 *result, t_vertex vector) {
    t_mat4 matrice = IDENTITY_MAT4;
    matrice[0][0] = vector.x;
    matrice[1][1] = vector.y;
    matrice[2][2] = vector.z;
    memcpy(result, &matrice, sizeof(t_mat4));
}

void perspective(float fov, float ratio, float near, float far, t_mat4 *result) {
    float   scale = 1.0f / tanf((fov / 2.0f) * (PI / 180.0f));
    t_mat4 matrix = {{scale / ratio,    0,      0,      0}
                    ,{0,                scale,  0,      0}
                    ,{0,                0,      -((far + near) / (far - near)),   -((2 * far * near) / (far - near))}
                    ,{0,                0,     -1,      0}};
    memcpy(result, &matrix, sizeof(t_mat4));
}

void rotate(t_mat4 matrix, float angle, t_vertex vector, t_mat4 *result) {
    float co = cosf((PI / 180.0f) * angle);
    float si = sinf((PI / 180.0f) * angle);
    if (vector.x) {
        if (vector.x < 0) si = -si;
        t_mat4 rmatrix =    {{1,  0,  0,  0}
                            ,{0, co,-si,  0}
                            ,{0, si, co,  0}
                            ,{0,  0,  0,  1}};
        mat4Mult(matrix, rmatrix, result);
    }
    else if (vector.y) {
        if (vector.y < 0) si = -si;
        t_mat4 rmatrix =    {{co, 0, si,  0}
                            ,{0,  1,  0,  0}
                            ,{-si,0, co,  0}
                            ,{0,  0,  0,  1}};
        mat4Mult(matrix, rmatrix, result);
    }
    else if (vector.z) {
        if (vector.z < 0) si = -si;
        t_mat4 rmatrix =    {{co,-si, 0,  0}
                            ,{si,co,  0,  0}
                            ,{0,  0,  1,  0}
                            ,{0,  0,  0,  1}};
        mat4Mult(matrix, rmatrix, result);
    }
}