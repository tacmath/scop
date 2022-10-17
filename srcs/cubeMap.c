#include "scop.h"

GLuint cubeMapTexInit(GLuint resolution) {
    GLuint cubeMapId;

    glGenTextures(1, &cubeMapId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapId);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (unsigned int n = 0; n < 6; ++n) 
    	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + n, 0, GL_RGB16F, 
                 resolution, resolution, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return (cubeMapId);
}

void getViewMatrixForCapture(t_mat4 *captureViews) {
	t_mat4 View = IDENTITY_MAT4;

	mat4Traslate(&View, (t_vertex){0.0f, 0.0f, -2.0f});

	rotate(View, 90.0f, (t_vertex){0.0f, -1.0f, 0.0f}, &captureViews[0]);	//x+
	rotate(View, 90.0f, (t_vertex){0.0f, 1.0f, 0.0f}, &captureViews[1]);	//x-
	rotate(View, 90.0f, (t_vertex){-1.0f, 0.0f, 0.0f}, &captureViews[2]);	//y+
	rotate(View, 90.0f, (t_vertex){1.0f, 0.0f, 0.0f}, &captureViews[3]);	//y-
	memcpy(captureViews[4], View, sizeof(t_mat4));							//z+
	rotate(View, 180.0f, (t_vertex){0.0f, 1.0f, 0.0f}, &captureViews[5]);	//z-
}

GLuint captureCubeMapFromRender(GLuint cubeMapId, GLuint viewLoc, GLuint resolution) {
	t_mat4 captureViews[6];

	getViewMatrixForCapture(captureViews);

	glViewport(0, 0, resolution, resolution);
	glEnable(GL_DEPTH_TEST);
	for (unsigned int n = 0; n < 6; ++n)
	{
		glUniformMatrix4fv(viewLoc, 1, GL_TRUE, (GLfloat*)(captureViews[n]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
							GL_TEXTURE_CUBE_MAP_POSITIVE_X + n, cubeMapId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	return (cubeMapId);
}

GLuint generateSkyBoxFromEquirectangular(GLuint hdrTexture, char *path, GLuint cubeVAO) {
	t_mat4 captureProjection;
	GLuint cubeMapId, programShader; 

	if (!(programShader = initShaders("shaders/basicVS.glsl", "shaders/EquirectangularToCubemapFS.glsl", path)))
		return (0);
	cubeMapId = cubeMapTexInit(CUBE_MAP_RESOLUTION);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBE_MAP_RESOLUTION, CUBE_MAP_RESOLUTION);
	perspective(90.0f, 1.0f, 0.1f, 10.0f, &captureProjection);
	glBindVertexArray(cubeVAO);
	glUseProgram(programShader);
	glUniformMatrix4fv(glGetUniformLocation(programShader, "projection"), 1, GL_TRUE, (GLfloat*)captureProjection);
	glUniform1i(glGetUniformLocation(programShader, "equirectangularMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	captureCubeMapFromRender(cubeMapId, glGetUniformLocation(programShader, "view"), CUBE_MAP_RESOLUTION);
	glDeleteProgram(programShader);
	return (cubeMapId);
}

GLuint generateIrradianceFromSkyBox(GLuint skybox, char *path, GLuint cubeVAO) {
	t_mat4 captureProjection;
	GLuint cubeMapId, programShader; 

	if (!(programShader = initShaders("shaders/basicVS.glsl", "shaders/irradianceFS.glsl", path)))
		return (0);
	cubeMapId = cubeMapTexInit(IRRADIANCE_MAP_RESOLUTION);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, IRRADIANCE_MAP_RESOLUTION, IRRADIANCE_MAP_RESOLUTION);
	perspective(90.0f, 1.0f, 0.1f, 10.0f, &captureProjection);
	glBindVertexArray(cubeVAO);
	glUseProgram(programShader);
	glUniformMatrix4fv(glGetUniformLocation(programShader, "projection"), 1, GL_TRUE, (GLfloat*)captureProjection);
	glUniform1i(glGetUniformLocation(programShader, "skybox"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	captureCubeMapFromRender(cubeMapId, glGetUniformLocation(programShader, "view"), IRRADIANCE_MAP_RESOLUTION);
	glDeleteProgram(programShader);
	return (cubeMapId);
}

GLuint createCubeMapFromEquirectangular(t_texture texture, char *path, GLuint cubeVAO, GLuint *irradianceMap) {
	GLuint hdrTexture, cubeMapId, captureFBO, captureRBO;

	if (!texture.data)
		return (0);
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, texture.x, texture.y, 0, GL_RGB, GL_FLOAT, texture.data); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	cubeMapId = generateSkyBoxFromEquirectangular(hdrTexture, path, cubeVAO);
#ifdef IRRADIANCE_MAP
	*irradianceMap = generateIrradianceFromSkyBox(cubeMapId, path, cubeVAO);
#endif

	glDeleteFramebuffers(1,  &captureFBO);
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteTextures(1, &hdrTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return (cubeMapId);
}

int initBackground(t_scop *scop) {

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};
    t_texture texture;
    pthread_t	thread;

    scop->background.VAO = initVertexArray((t_array){skyboxVertices, 8});
    initElementArray(scop->background.VAO, (t_array){skyboxIndices, 36});
    if (!(scop->background.programShader = initShaders("shaders/skyBoxVS.glsl", "shaders/skyBoxFS.glsl", scop->path)))
        return (0);

    pthread_create(&thread, 0, &loadCubeMap, scop);
    return (1);
}