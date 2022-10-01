#include "scop.h"

extern float cameraPosZ;

void setMatrix(t_scop *scop, GLuint matrixLoc, t_vertex *rotation) {
    t_mat4 matrix = IDENTITY_MAT4;
    t_mat4 test = IDENTITY_MAT4;
    t_mat4 model = IDENTITY_MAT4;
    t_mat4 view = IDENTITY_MAT4;
   
    rotate(matrix, rotation->y, (t_vertex){0.0f, 1.0f, 0.0f}, &model);
    rotate(model, rotation->x, (t_vertex){cos(scop->object.rotation.y * (PI / 180.0f)), 0.0f, sin(scop->object.rotation.y * (PI / 180.0f))}, &test);
    memcpy(&model, &test, sizeof(t_mat4));
    mat4Traslate(&matrix, (t_vertex){
                /*- ((scop->object.mesh.max.x - scop->object.mesh.min.x) / 2) - scop->object.mesh.min.x*/0.0f,
                - ((scop->object.mesh.max.y - scop->object.mesh.min.y) / 2) - scop->object.mesh.min.y,
                - ((scop->object.mesh.max.z - scop->object.mesh.min.z) / 2) - scop->object.mesh.min.z});
    mat4Mult(model, matrix, &test);
    memcpy(&model, &test, sizeof(t_mat4));

    mat4Traslate(&view, (t_vertex){0.0f, 0.0f, cameraPosZ - (scop->object.mesh.max.y - scop->object.mesh.min.y) * 2});

    mat4Mult(scop->projection, view, &matrix);
    memcpy(&view, &matrix, sizeof(t_mat4));
    mat4Mult(view, model, &matrix);

    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, (GLfloat*)matrix);
}

void mainLoop(t_scop *scop) {
    GLuint matrixLoc =  glGetUniformLocation(scop->object.programShader, "matrix");
    GLuint backgroundTexUni =    glGetUniformLocation(scop->background.programShader, "backgroundTex");
    double oldTime = glfwGetTime();
    double newTime;
   
    glUseProgram(scop->background.programShader);
    glBindTexture(GL_TEXTURE_2D, scop->background.textureID);
    glUniform1i(backgroundTexUni, 0);
    initUniforms(&scop->object);
    glBindTexture(GL_TEXTURE_2D, scop->background.textureID);
    while ( glfwGetKey(scop->window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(scop->window) == 0 ) {
        glClear(GL_DEPTH_BUFFER_BIT);
        drawBackground(scop);
        drawObject(scop, matrixLoc);
		glfwSwapBuffers(scop->window);
        getEvents(scop);
        newTime = glfwGetTime();
        if ((newTime - oldTime) * 1000000 < (1000000.0 / MAX_FPS))
            usleep((1000000.0 / MAX_FPS) - (newTime - oldTime) * 100000);
        oldTime = glfwGetTime();
    }
}