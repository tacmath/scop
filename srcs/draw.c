#include "scop.h"

void initUniforms(t_object *object) {
    GLuint ObjectSizeLoc = glGetUniformLocation(object->programShader, "Osize");
    GLuint ObjectMinLoc = glGetUniformLocation(object->programShader, "Omin");
    GLuint tex0Uni = glGetUniformLocation(object->programShader, "tex0");
    t_vertex objectSize =   {object->mesh.max.x - object->mesh.min.x,
                            object->mesh.max.y - object->mesh.min.y,
                            object->mesh.max.z - object->mesh.min.z};

    glUseProgram(object->programShader);

    glUniform3fv(ObjectSizeLoc, 1, (void*)(&objectSize));
    glUniform3fv(ObjectMinLoc, 1, (void*)(&object->mesh.min));
    glBindTexture(GL_TEXTURE_2D, object->textureID);
    glUniform1i(tex0Uni, 0);
}

void drawBackground(t_scop *scop) {
        glDisable(GL_DEPTH_TEST);
        glUseProgram(scop->background.programShader);
        glBindVertexArray(scop->background.VAO);
        glBindTexture(GL_TEXTURE_2D, scop->background.textureID);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);
}

void drawObject(t_scop *scop, GLuint matrixLoc) {
    glUseProgram(scop->object.programShader);
    setMatrix(scop, matrixLoc, &scop->object.rotation);
    glBindVertexArray(scop->object.VAO);
    glBindTexture(GL_TEXTURE_2D, scop->object.textureID);
    if (scop->object.mesh.indices.size)
        glDrawElements(GL_TRIANGLES, scop->object.mesh.indices.size , GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, scop->object.mesh.vertices.size);
}