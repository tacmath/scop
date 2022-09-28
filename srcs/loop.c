#include "scop.h"

extern float cameraPosZ;

void setMatrix(t_scop *scop, GLuint matrixLoc, t_vertex *rotation) {
    t_mat4 matrix = IDENTITY_MAT4;
    t_mat4 test = IDENTITY_MAT4;
    t_mat4 model = IDENTITY_MAT4;
    t_mat4 view = IDENTITY_MAT4;

   

    rotate(matrix, rotation->y, (t_vertex){0.0f, 1.0f, 0.0f}, &model);
    rotate(model, rotation->x, (t_vertex){1.0f, 0.0f, 0.0f}, &test);
    memcpy(&model, &test, sizeof(t_mat4));
    mat4Traslate(&matrix, (t_vertex){0.0f, - ((scop->object.mesh.max.y - scop->object.mesh.min.y) / 2) - scop->object.mesh.min.y, - ((scop->object.mesh.max.z - scop->object.mesh.min.z) / 2) - scop->object.mesh.min.z});
    mat4Mult(model, matrix, &test);
    memcpy(&model, &test, sizeof(t_mat4));

    mat4Traslate(&view, (t_vertex){0.0f,0.0f /*- ((scop->object.mesh.max.y - scop->object.mesh.min.y) / 2) - scop->object.mesh.min.y*/, cameraPosZ - (scop->object.mesh.max.y - scop->object.mesh.min.y) * 2});

    mat4Mult(scop->projection, view, &matrix);
    memcpy(&view, &matrix, sizeof(t_mat4));
    mat4Mult(view, model, &matrix);

//    printMat4(matrix);

    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, (GLfloat*)matrix);
}

void getEvents(t_scop *scop) {
     int state;
     double posx, posy;

    
    glfwPollEvents();
    state = glfwGetMouseButton(scop->window, GLFW_MOUSE_BUTTON_LEFT);
    glfwGetCursorPos(scop->window, &posx, &posy);
    if (state == GLFW_PRESS)
    {
        if (fabs(scop->object.rotation.y + posx - scop->mouse.x) < 90)
            scop->object.rotation.y += posx - scop->mouse.x;
        if (fabs(scop->object.rotation.x + posy - scop->mouse.y) < 90)
            scop->object.rotation.x += posy - scop->mouse.y;
    }
    scop->mouse.x = posx;
    scop->mouse.y = posy;
}

void mainLoop(t_scop *scop) {
    GLuint matrixLoc = glGetUniformLocation(scop->programShader, "matrix");
    GLuint ObjectSizeLoc = glGetUniformLocation(scop->programShader, "Osize");
    GLuint ObjectMinLoc = glGetUniformLocation(scop->programShader, "Omin");
    GLuint tex0Uni = glGetUniformLocation(scop->programShader, "tex0");
    t_vertex objectSize =   {scop->object.mesh.max.x - scop->object.mesh.min.x,
                            scop->object.mesh.max.y - scop->object.mesh.min.y,
                            scop->object.mesh.max.z - scop->object.mesh.min.z};

    glUseProgram(scop->programShader);

    glUniform3fv(ObjectSizeLoc, 1, (void*)(&objectSize));
    glUniform3fv(ObjectMinLoc, 1, (void*)(&scop->object.mesh.min));

    glBindTexture(GL_TEXTURE_2D, scop->textureID);
    glUniform1i(tex0Uni, 0);
    glEnable(GL_DEPTH_TEST);

    while ( glfwGetKey(scop->window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(scop->window) == 0 ) {
        glClear(GL_DEPTH_BUFFER_BIT);

        setMatrix(scop, matrixLoc, &scop->object.rotation);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(scop->background.programShader);
        glBindVertexArray(scop->background.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(scop->programShader);
        glBindVertexArray(scop->object.VAO);
        glDrawElements(GL_TRIANGLES, scop->object.mesh.nbIndices , GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(scop->window);
        getEvents(scop);
        usleep(2000);
    }
}