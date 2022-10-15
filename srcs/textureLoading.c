#include "scop.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 
/*
    stbi_set_flip_vertically_on_load_thread(1);
    if (!(texture.data = stbi_load(fileName, &texture.x, &texture.y, &texture.numColCh, 4))) {
        dprintf(2, "Failed to load %s\n", fileName);
        return (0);
    }*/

GLuint textureInit(t_texture texture) {
    GLuint      textureID;

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

    glBindTexture(GL_TEXTURE_CUBE_MAP, scop->background.textureID);
    for (int n = 0; n < 6; n++) {
        if (scop->textures.cubeMap[n].status == LOADED) {
            texture = scop->textures.cubeMap[n].texture;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + n, 0, GL_RGB, texture.x, texture.y, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data);
            scop->textures.cubeMap[n].status = 0;
            scop->textures.texturesLeft -= 1;
            dprintf(1, "new texture loaded\n");
            stbi_image_free(texture.data);   
        }
    }
}

void enableOptionalTextures(t_scop *scop) {
    glUseProgram(scop->object.programShader);
    for (int n = 0; n < scop->object.segmentNb; n++) {
        if (scop->object.segments[n].normalTextureID) {
            glUniform1i(glGetUniformLocation(scop->object.programShader, "hasNormalMap"), 1);
            if (scop->object.segments[n].metalTextureID && scop->object.segments[n].routhTextureID)
                glUniform1i(glGetUniformLocation(scop->object.programShader, "activatePBR"), 1);
            break ;
        }
    }
    glUseProgram(0);
}

void bindTexture(t_scop *scop, t_textureInfo *textureInfo, GLuint textureIndex, GLuint segmentIndex) {
    if (textureIndex == 0) {
        if (textureInfo->texture.data)
            scop->object.segments[segmentIndex].textureID = textureInit(textureInfo->texture);
        else
            scop->object.segments[segmentIndex].textureID = scop->textures.defaultTextureID;
        return ;
    }
    if (textureIndex == 1) {
        scop->object.segments[segmentIndex].normalTextureID = textureInit(textureInfo->texture);
        return ;
    }
    if (textureIndex == 2) {
        scop->object.segments[segmentIndex].metalTextureID = textureInit(textureInfo->texture);
        return ;
    }
    if (textureIndex == 3) {
        scop->object.segments[segmentIndex].routhTextureID = textureInit(textureInfo->texture);
        return ;
    }
    if (textureIndex == 4) {
        scop->object.segments[segmentIndex].AOTextureID = textureInit(textureInfo->texture);
        return ;
    }
}

void bindTextures(t_scop *scop) {
    t_textureInfo *textureInfo;
 //   GLuint *texturesID;

    for (GLuint n = 0; n < scop->object.segmentNb; n++) {
        for (GLuint m = 0; m < TEX_PER_SEGMENT; m++) {
            textureInfo = &(scop->textures.object[n * TEX_PER_SEGMENT + m]);
            if (textureInfo->status == LOADED) {
              /*  texturesID = (void*)(&(scop->object.segments[n]));*/
                bindTexture(scop, textureInfo, m, n);
                textureInfo->status = 0;
                scop->textures.texturesLeft -= TEX_OBJECT_VALUE;
                dprintf(1, "new texture loaded\n");
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
    char **texturesName;
    t_texture texture;
    t_scop *scop;

    scop = data;
    if (!(texturesName = malloc(sizeof(char*) * 6)))
        return (0);

    texturesName[0] = ft_strjoin(scop->path, CUBE_MAP_PX);
    texturesName[1] = ft_strjoin(scop->path, CUBE_MAP_NX);
    texturesName[2] = ft_strjoin(scop->path, CUBE_MAP_PY);
    texturesName[3] = ft_strjoin(scop->path, CUBE_MAP_NY);
    texturesName[4] = ft_strjoin(scop->path, CUBE_MAP_PZ);
    texturesName[5] = ft_strjoin(scop->path, CUBE_MAP_NZ);
    
    stbi_set_flip_vertically_on_load_thread(0);
    for (int n = 0; n < 6; n++) {
        texture.data = stbi_load(texturesName[n], &texture.x, &texture.y, &texture.numColCh, 0);
        if (!texture.data)
            dprintf(2, "Failed to load %s\n", texturesName[n]);
        scop->textures.texturesLeft += 1;
        scop->textures.cubeMap[n].texture = texture;
        scop->textures.cubeMap[n].status = LOADED;
        dprintf(1, "texture %s parsed\n", texturesName[n]);
    }
    for (int n = 0; n < 6; n++)
        free(texturesName[n]);
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
    dprintf(2, "texture %s parsed\n", fileName);
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
    stbi_set_flip_vertically_on_load_thread(1);
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
        dprintf(1, "texture %s parsed\n", fileName);
    }
    loadAllOptionalTextures(textures);
    for (int n = 0; n < textures->segmentNb; n++)
        free(textures->texturesName[n]);
    free(textures->texturesName);
}

int loadAllTextures(t_scop *scop) {
    pthread_t	thread;
    char *defaultTexture;
    t_texture texture;
    GLuint textureID;
    
    stbi_set_flip_vertically_on_load_thread(1);
    if (scop->option.texture && (defaultTexture = ft_strjoin(scop->path, scop->option.texture))) {
        if (!(texture.data = stbi_load(defaultTexture, &texture.x, &texture.y, &texture.numColCh, 4))) {
            dprintf(2, "Failed to load %s\n", defaultTexture);
            free(defaultTexture);
            return (0);
        }
        textureID = textureInit(texture);
        for (int n = 0; n < scop->object.segmentNb; n++) {
            free(scop->object.mesh.segments[n].texture);
            scop->object.segments[n].textureID = textureID;
        }
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
    for (int n = 0; n < scop->object.segmentNb; n++)
        if (scop->object.mesh.segments[n].texture)
            scop->textures.texturesName[n] = scop->object.mesh.segments[n].texture;
    scop->textures.segmentNb = scop->object.segmentNb;
    pthread_create(&thread, 0, loadAllTexturesThread, &scop->textures);
    return (1);
}