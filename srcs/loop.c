#include "scop.h"

extern float cameraPosZ;

void setMatrix(t_scop *scop, GLuint matrixLoc, GLuint modelMatrixLoc) {
    t_mat4 matrix = IDENTITY_MAT4;
    t_mat4 model = IDENTITY_MAT4;
    t_mat4 view = IDENTITY_MAT4;

    mat4Traslate(&matrix, (t_vertex){
                /*- ((scop->object.mesh.max.x - scop->object.mesh.min.x) / 2) - scop->object.mesh.min.x*/0.0f,
                - ((scop->object.mesh.max.y - scop->object.mesh.min.y) / 2) - scop->object.mesh.min.y,
                - ((scop->object.mesh.max.z - scop->object.mesh.min.z) / 2) - scop->object.mesh.min.z});
    mat4Mult(scop->rotation, matrix, &model);

    mat4Traslate(&view, (t_vertex){0.0f, 0.0f, cameraPosZ - (scop->object.mesh.max.y - scop->object.mesh.min.y) * 2});
  //  mat4Mult(view, scop->rotation, &model);  //pour bougée la caméra a la place de l'object
    mat4Mult(scop->projection, view, &matrix);

    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, (GLfloat*)matrix);
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, (GLfloat*)model);
}

void mainLoop(t_scop *scop) {
    GLuint matrixLoc =  glGetUniformLocation(scop->object.programShader, "matrix");
    GLuint cameraPosLoc =  glGetUniformLocation(scop->object.programShader, "cameraPos");
    GLuint modelMatrixLoc =  glGetUniformLocation(scop->object.programShader, "model");
    GLuint rotatioinMatrixLoc =  glGetUniformLocation(scop->object.programShader, "rotation");
    GLuint transitionLoc = glGetUniformLocation(scop->object.programShader, "transition");
    double oldTime = glfwGetTime();
    double newTime;
   
    glUseProgram(scop->background.programShader);
    glBindTexture(GL_TEXTURE_2D, scop->background.textureID);
    glUniform1i(glGetUniformLocation(scop->background.programShader, "backgroundTex"), 0);
    initUniforms(&scop->object);
    glUniform3fv(glGetUniformLocation(scop->object.programShader, "lightPos"), 1, (void*)(&scop->lightPos));
    glUniform3fv(glGetUniformLocation(scop->object.programShader, "lightColor"), 1, (void*)(&scop->lightColor));
    while ( glfwGetKey(scop->window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(scop->window) == 0 ) {
        glClear(GL_DEPTH_BUFFER_BIT);
        drawBackground(scop);
        drawObject(scop, matrixLoc, modelMatrixLoc);
		glfwSwapBuffers(scop->window);
        getEvents(scop);
        glUniform1f(transitionLoc, scop->transition);
        glUniformMatrix4fv(rotatioinMatrixLoc, 1, GL_TRUE, (GLfloat*)(scop->rotation));
        t_vertex carmeraPos = (t_vertex){0.0f, 0.0f, cameraPosZ - (scop->object.mesh.max.y - scop->object.mesh.min.y) * 2};
        glUniform3fv(cameraPosLoc, 1, (void*)(&carmeraPos));

        newTime = glfwGetTime();
        if ((newTime - oldTime) * 1000000 < (1000000.0 / MAX_FPS))
            usleep((1000000.0 / MAX_FPS) - (newTime - oldTime) * 100000);
        oldTime = glfwGetTime();
    }
}