#include "scop.h" 

extern float cameraPosZ;

void setMatrix(GLuint matrixLoc, float rotation) {
    t_mat4 matrix = IDENTITY_MAT4;
//    t_mat4 test = IDENTITY_MAT4;
    t_mat4 model = IDENTITY_MAT4;
    t_mat4 view = IDENTITY_MAT4;
    t_mat4 proj = IDENTITY_MAT4;

   

    rotate(matrix, rotation, (t_vertex){0.0f, 1.0f, 0.0f}, &model);
  /*  mat4Traslate(&matrix, (t_vertex){0.0f, 0.0f, -1.5f});
    mat4Mult(model, matrix, &test);
    memcpy(&model, &test, sizeof(t_mat4));*/

    mat4Traslate(&view, (t_vertex){0.0f, -0.5f, cameraPosZ});

    perspective(45.0f, (float)(400.0f/300.0f), 0.1f, 100.0f, &proj);

    mat4Mult(proj, view, &matrix);
    memcpy(&view, &matrix, sizeof(t_mat4));
    mat4Mult(view, model, &matrix);

   // printMat4(matrix);

    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, (GLfloat*)matrix);
}

void mainLoop(t_scop *scop) {
    GLuint matrixLoc = glGetUniformLocation(scop->programShader, "matrix");
    GLuint tex0Uni = glGetUniformLocation(scop->programShader, "tex0");

    float rotation = 0;
    double prevTime = glfwGetTime();

    glBindVertexArray(scop->object.VAO);
    glBindTexture(GL_TEXTURE_2D, scop->textureID);
    glUseProgram(scop->programShader);
    glUniform1i(tex0Uni, 0);
    glEnable(GL_DEPTH_TEST);

    while ( glfwGetKey(scop->window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(scop->window) == 0 ) {


        double crtTime = glfwGetTime();
        if (crtTime - prevTime >=  1 / 60) {
            prevTime = crtTime;
            rotation += 0.5f;
        }

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setMatrix(matrixLoc, rotation);
    //    glDrawArrays(GL_POINTS, 0, scop->mesh.nbVertices);
        glDrawElements(GL_TRIANGLES, scop->object.mesh.nbIndices , GL_UNSIGNED_INT, 0);
    //    glDrawElements(GL_POINTS, scop->mesh.nbTriangleIndices * 3, GL_UNSIGNED_INT, scop->mesh.triangleIndices);
		glfwSwapBuffers(scop->window);
        glfwPollEvents();
        usleep(2000);
    }
}