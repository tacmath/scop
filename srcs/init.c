#include "scop.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

int initWindow(t_scop *scop) {
    char path[1024];
    getcwd(path, 1024);
    if(!glfwInit()) {
        dprintf(2, "Glfw failed to init\n");
        return (0);
    }
    chdir(path);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux ; ne devrait pas être nécessaire
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
    if(!(scop->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Scop", NULL, NULL))) {
        glfwTerminate();
        dprintf(2, "Glfw failed to create a window\n");
        return (0);
    }
    glfwMakeContextCurrent(scop->window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        dprintf(2, "Glew failed to init\n");
        return (0);
    }	
    glfwSetInputMode(scop->window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetScrollCallback(scop->window, scroll_callback);
//    printf("%s\n", glGetString(GL_VERSION));
    return (1);
}

GLuint initShaders(char *vertexShaderFile, char *fragmentShaderFile, char *path) {
    const char* vertexShaderSource = getShaderSource((vertexShaderFile = ft_strjoin(path, vertexShaderFile)));
    const char* fragmentShaderSource = getShaderSource((fragmentShaderFile = ft_strjoin(path, fragmentShaderFile)));
    free(vertexShaderFile);
    free(fragmentShaderFile);
    if (!vertexShaderSource || !fragmentShaderSource)
        return (0);
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint programShader = glCreateProgram();
	glAttachShader(programShader, vertexShader);
	glAttachShader(programShader, fragmentShader);
	glLinkProgram(programShader);

    GLuint status;
    glGetProgramiv(programShader, GL_LINK_STATUS, &status);
    if (!status)
        dprintf(2, "program shader failed to link\n");

    free((void*)vertexShaderSource);
    free((void*)fragmentShaderSource);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    return (programShader);
}

GLuint textureInit(char *fileName) {
    t_texture   texture;
    GLuint      textureID;

    stbi_set_flip_vertically_on_load(1);
    if (!(texture.data = stbi_load(fileName, &texture.x, &texture.y, &texture.numColCh, 4))) {
        dprintf(2, "Failed to load %s\n", fileName);
        return (0);
    }
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.x, texture.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
   // glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(texture.data);
    return (textureID);

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

    char **texturesName;

    if (!(texturesName = malloc(sizeof(char*) * 6)))
        return (0);

    texturesName[0] = ft_strjoin(scop->path, "texture/TXR_ENV_Skybox_Cloud Layers__Cam_2_Left+X.png");
    texturesName[1] = ft_strjoin(scop->path, "texture/TXR_ENV_Skybox_Cloud Layers__Cam_3_Right-X.png");
    texturesName[2] = ft_strjoin(scop->path, "texture/TXR_ENV_Skybox_Cloud Layers__Cam_4_Up+Y.png");
    texturesName[3] = ft_strjoin(scop->path, "texture/TXR_ENV_Skybox_Cloud Layers__Cam_5_Down-Y.png");
    texturesName[4] = ft_strjoin(scop->path, "texture/TXR_ENV_Skybox_Cloud Layers__Cam_0_Front+Z.png");
    texturesName[5] = ft_strjoin(scop->path, "texture/TXR_ENV_Skybox_Cloud Layers__Cam_1_Back-Z.png");
    

    scop->background.VAO = initVertexArray((t_array){skyboxVertices, 8});
    initElementArray(scop->background.VAO, (t_array){skyboxIndices, 36});

    glGenTextures(1, &scop->background.textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, scop->background.textureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(0);
    for (int n = 0; n < 6; n++) {
        int x, y, comp;
        char *data = stbi_load(texturesName[n], &x, &y, &comp, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + n, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
            dprintf(2, "failed to load %s\n", texturesName[n]);
        free(texturesName[n]);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    if (!(scop->background.programShader = initShaders("shaders/backgroundVS", "shaders/backgroundFS", scop->path)))
        return (0);
    return (1);
}
