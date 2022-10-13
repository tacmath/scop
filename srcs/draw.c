#include "scop.h"

void drawBackground(t_scop *scop) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(scop->background.programShader);
    glBindVertexArray(scop->background.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, scop->background.textureID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDepthFunc(GL_LESS);
}

void drawObject(t_scop *scop, GLuint modelMatrixLoc) {
    glUseProgram(scop->object.programShader);
    setModelMatrix(scop, modelMatrixLoc);
    for (int n = 0; n < scop->object.segmentNb; n++) {
        glBindVertexArray(scop->object.segments[n].VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scop->object.segments[n].textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, scop->object.segments[n].normalTextureID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, scop->object.segments[n].metalTextureID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, scop->object.segments[n].routhTextureID);
        if (scop->object.mesh.indices.size)
            glDrawElements(GL_TRIANGLES, scop->object.mesh.indices.size , GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, scop->object.mesh.vertices.size);
    }
}