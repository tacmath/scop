#include "scop.h"

extern float cameraPosZ;

static void initUniforms(t_scop *scop) {
    t_vertex objectSize =   {scop->object.mesh.max.x - scop->object.mesh.min.x,
                            scop->object.mesh.max.y - scop->object.mesh.min.y,
                            scop->object.mesh.max.z - scop->object.mesh.min.z};


    glUseProgram(scop->background.programShader);
    glUniform1i(glGetUniformLocation(scop->background.programShader, "skybox"), 0);

    glUseProgram(scop->object.programShader);
    glUniform3fv(glGetUniformLocation(scop->object.programShader, "Osize"), 1, (void*)(&objectSize));
    glUniform3fv(glGetUniformLocation(scop->object.programShader, "Omin"), 1, (void*)(&scop->object.mesh.min));
    glUniform3fv(glGetUniformLocation(scop->object.programShader, "lightPos"), 1, (void*)(&scop->lightPos));
    glUniform1i(glGetUniformLocation(scop->object.programShader, "activateNormalMap"), 1);
    glUniform1i(glGetUniformLocation(scop->object.programShader, "Texture"), 0); 
    glUniform1i(glGetUniformLocation(scop->object.programShader, "NormalMap"), 1);
}

void setModelMatrix(t_scop *scop, GLuint matrixLoc) {
    t_mat4 matrix = IDENTITY_MAT4;

    mat4Traslate(&matrix, (t_vertex){
                /*- ((scop->object.mesh.max.x - scop->object.mesh.min.x) / 2) - scop->object.mesh.min.x*/0.0f,
                - ((scop->object.mesh.max.y - scop->object.mesh.min.y) / 2) - scop->object.mesh.min.y,
                - ((scop->object.mesh.max.z - scop->object.mesh.min.z) / 2) - scop->object.mesh.min.z});
    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, (GLfloat*)matrix);
}

static void setVPMatrix(t_scop *scop, GLuint matrixLoc) {
    t_mat4 matrix = IDENTITY_MAT4;
    t_mat4 view = IDENTITY_MAT4;

    mat4Traslate(&view, (t_vertex){0.0f, 0.0f, cameraPosZ - (scop->object.mesh.max.y - scop->object.mesh.min.y) * 2});
    mat4Mult(view, scop->rotation, &matrix);
    mat4Mult(scop->projection, matrix, &view);
    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, (GLfloat*)view);
}

static void moveCamera(t_scop *scop) {
    static GLuint backgroundMatrixLoc = 0;
    static GLuint cameraPosLoc = 0;
    static GLuint matrixLoc = 0;
    t_mat4 matrix;
    t_vertex carmeraPos;

    if (!matrixLoc) {
        backgroundMatrixLoc = glGetUniformLocation(scop->background.programShader, "matrix");
        cameraPosLoc =        glGetUniformLocation(scop->object.programShader, "cameraPos");
        matrixLoc =           glGetUniformLocation(scop->object.programShader, "matrix");
    }
    
    memcpy(matrix, scop->rotation, sizeof(t_mat4));
    mat4Transpose(&matrix);
    carmeraPos = mat4Vec3Mult(matrix, (t_vertex){0.0f, 0.0f, -(cameraPosZ - (scop->object.mesh.max.y - scop->object.mesh.min.y) * 2)});
    glUniform3fv(cameraPosLoc, 1, (void*)(&carmeraPos));
//    glUniform3fv(glGetUniformLocation(scop->object.programShader, "lightPos"), 1, (void*)(&carmeraPos));
    setVPMatrix(scop, matrixLoc);
    
    mat4Mult(scop->projection, scop->rotation, &matrix);
    glUseProgram(scop->background.programShader);
    glUniformMatrix4fv(backgroundMatrixLoc, 1, GL_TRUE, (GLfloat*)matrix);
}

void mainLoop(t_scop *scop) {
    GLuint modelMatrixLoc =     glGetUniformLocation(scop->object.programShader, "model");

    initUniforms(scop);
    glEnable(GL_DEPTH_TEST);
    while ( glfwGetKey(scop->window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(scop->window) == 0 ) {
        glClear(GL_DEPTH_BUFFER_BIT);
        moveCamera(scop);
        drawBackground(scop);
        drawObject(scop, modelMatrixLoc);
		glfwSwapBuffers(scop->window);
        getEvents(scop);
        limitFPS();
    }
}