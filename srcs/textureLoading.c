#include "scop.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

GLuint textureInit(t_texture texture) {
    GLuint      textureID;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.x, texture.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
 //   glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(texture.data);
    return (textureID);
}

void bindCubeMap(t_scop *scop) {
    t_texture texture;
    t_cubeMapTextures cubeMapTextures;

    if (scop->textures.cubeMap.status == LOADED) {
        texture = scop->textures.cubeMap.texture;
        cubeMapTextures = createCubeMapFromEquirectangular(texture, scop->path, scop->background.VAO, scop->option.IBL);
        scop->background.textureID = cubeMapTextures.evironementID;
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextures.irradianceID);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextures.prefillerID);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, cubeMapTextures.brdfID);
        scop->textures.cubeMap.status = 0;
        scop->textures.texturesLeft -= 1;
    //    dprintf(1, "new texture loaded\n");
        stbi_image_free(texture.data);   
    }
}

void enableOptionalTextures(t_scop *scop) {
    glUseProgram(scop->object.programShader);
    if (scop->object.segments[0].normalTextureID)
        glUniform1i(glGetUniformLocation(scop->object.programShader, "hasNormalMap"), 1);
    if (scop->object.segments[0].metalTextureID && scop->object.segments[0].routhTextureID)
        glUniform1i(glGetUniformLocation(scop->object.programShader, "activatePBR"), 1);
    glUseProgram(0);
}

void bindTextures(t_scop *scop) {
    t_textureInfo *textureInfo;
    GLuint *texturesID;

    for (GLuint n = 0; n < scop->object.segmentNb; n++) {
        for (GLuint m = 0; m < TEX_PER_SEGMENT; m++) {
            textureInfo = &(scop->textures.object[n * TEX_PER_SEGMENT + m]);
            if (textureInfo->status == LOADED) {
                texturesID = (void*)&scop->object.segments[n];
                if (textureInfo->texture.data)
                    texturesID[m] = textureInit(textureInfo->texture);
                else if (m == 0)
                    texturesID[0] = scop->textures.defaultTextureID;
                textureInfo->status = 0;
                scop->textures.texturesLeft -= TEX_OBJECT_VALUE;
            //    dprintf(1, "new texture loaded\n");
            }
        }
    }
}

void bindAllTextures(t_scop *scop) {
    if (!scop->textures.texturesLeft)
        return ;
 //   printf("textureLeft = %d\n", scop->textures.texturesLeft);
    if (SKYBOX_TEXTURE(scop->textures.texturesLeft))
        bindCubeMap(scop);
    if (scop->textures.texturesLeft >= TEX_OBJECT_VALUE) {
        bindTextures(scop);
        enableOptionalTextures(scop);
    }
}

void *loadCubeMap(void *data) {
    char *texturesName;
    t_texture texture;
    t_scop *scop;

    scop = data;

    texturesName = CUBE_MAP_FILE;
    if (scop->option.background)
        texturesName = scop->option.background;
    texturesName = ft_strjoin(scop->path, texturesName);
    texture.data = stbi_loadf(texturesName, &texture.x, &texture.y, &texture.numColCh, 0);
    if (!texture.data)
        dprintf(2, "Failed to load %s\n", texturesName);
    scop->textures.texturesLeft += 1;
    scop->textures.cubeMap.texture = texture;
    scop->textures.cubeMap.status = LOADED;
    //    dprintf(1, "texture %s parsed\n", texturesName[n]);
    free(texturesName);
    return (0);
}

void loadOptionalTexture(t_textureInfo *textureInfo, unsigned int *texturesLeft, char *originalName, char *type) {
    char *fileName;
    t_texture texture;

    if (!originalName || !(fileName = ft_replaceStr(originalName, "Base_color", type)))
        return ;
    if (!(texture.data = stbi_load(fileName, &texture.x, &texture.y, &texture.numColCh, 4))) {
        dprintf(2, "Failed to load %s\n", fileName);
        free(fileName);
        return ;
    }
    textureInfo->texture = texture;
    *texturesLeft += TEX_OBJECT_VALUE;
    textureInfo->status = LOADED;
   // dprintf(2, "texture %s parsed\n", fileName);
    free(fileName);
}

void loadAllOptionalTextures(t_textureLoader *textures) {
    char optionalTextureName[4][15] = {"Normal_OpenGL", "Metallic", "Roughness", "Mixed_AO"};

    for (unsigned int n = 0; n < 4; n++)
        for (unsigned int m = 0; m < textures->segmentNb; m++)
            loadOptionalTexture(&(textures->object[n + 1 + TEX_PER_SEGMENT * m]), &textures->texturesLeft,
                                textures->texturesName[m], optionalTextureName[n]);
}

void *loadAllTexturesThread(void *data) {
    t_textureLoader *textures;
    t_texture texture;
    char    *fileName;

    textures = data;
    for (int n = 0; n < textures->segmentNb; n++) {
        fileName = textures->texturesName[n];
        if (!fileName) {
            textures->texturesLeft += TEX_OBJECT_VALUE;
            textures->object[TEX_PER_SEGMENT * n].status = LOADED;
            continue ;
        }
        texture.data = stbi_load(fileName, &texture.x, &texture.y, &texture.numColCh, 4);
        if (!texture.data)
            dprintf(2, "Failed to load %s\n", fileName);
        textures->texturesLeft += TEX_OBJECT_VALUE;
        textures->object[TEX_PER_SEGMENT * n].texture = texture;
        textures->object[TEX_PER_SEGMENT * n].status = LOADED;
    //    dprintf(1, "texture %s parsed\n", fileName);
    }
    loadAllOptionalTextures(textures);
    for (int n = 0; n < textures->segmentNb; n++)
        free(textures->texturesName[n]);
    free(textures->texturesName);
    return (0);
}

int loadAllTextures(t_scop *scop) {
    pthread_t	thread;
    char *defaultTexture;
    t_texture texture;
    GLuint textureID;
    
    stbi_set_flip_vertically_on_load(1);
    if (scop->option.texture && (defaultTexture = ft_strjoin(scop->path, scop->option.texture))) {
        if (!(texture.data = stbi_load(defaultTexture, &texture.x, &texture.y, &texture.numColCh, 4))) {
            dprintf(2, "Failed to load %s\n", defaultTexture);
            free(defaultTexture);
            return (0);
        }
        textureID = textureInit(texture);
        for (int n = 0; n < scop->object.segmentNb; n++)
            scop->object.segments[n].textureID = textureID;
        free(defaultTexture);
        return (1);
    }
    if (!(scop->textures.object = calloc(sizeof(t_textureInfo) * TEX_PER_SEGMENT, scop->object.segmentNb)))
        return (0);
    if ((defaultTexture = ft_strjoin(scop->path, DEFAULT_TEXTURE))
        && (texture.data = stbi_load(defaultTexture, &texture.x, &texture.y, &texture.numColCh, 4)))
        scop->textures.defaultTextureID = textureInit(texture);
    free(defaultTexture);
    if (!(scop->textures.texturesName = calloc(sizeof(char*), scop->object.segmentNb)))
        return (0);
    for (int n = 0; n < scop->object.segmentNb; n++) {
            scop->textures.texturesName[n] = scop->object.mesh.segments[n].texture;
            scop->object.mesh.segments[n].texture = 0;
    }
    scop->textures.segmentNb = scop->object.segmentNb;
    pthread_create(&thread, 0, loadAllTexturesThread, &scop->textures);
    return (1);
}