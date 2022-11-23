#include "scop.h"

void drawBackground(t_scop *scop) {
    glDepthFunc(GL_LEQUAL);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, scop->background.textureID);
    glUseProgram(scop->background.programShader);
    glBindVertexArray(scop->background.VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDepthFunc(GL_LESS);
}

void setSegmentTextures(t_segment *segment) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, segment->textureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, segment->normalTextureID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, segment->metalTextureID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, segment->routhTextureID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, segment->AOTextureID);
}

void drawObject(t_scop *scop) {
#ifdef FACE_CULLING
    glEnable(GL_CULL_FACE);
#endif
    glUseProgram(scop->object.programShader);
    for (int n = 0; n < scop->object.segmentNb; n++) {
        glBindVertexArray(scop->object.segments[n].VAO);
        setSegmentTextures(&scop->object.segments[n]);
        glDrawArrays(GL_TRIANGLES, 0, scop->object.mesh.vertices.size);
    }
#ifdef FACE_CULLING
    glDisable(GL_CULL_FACE);
#endif
}