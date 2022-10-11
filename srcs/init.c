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

    free((void*)vertexShaderSource);
    free((void*)fragmentShaderSource);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    return (programShader);
}

GLuint textureInit(char *fileName, char *path) {
    t_texture   texture;
    GLuint      textureID;

    stbi_set_flip_vertically_on_load(1);
    fileName = ft_strjoin(path, fileName);
    if (!(texture.data = stbi_load(fileName, &texture.x, &texture.y, &texture.numColCh, 4))) {
        dprintf(2, "Failed to load %s\n", fileName);
        free(fileName);
        return (0);
    }
    free(fileName);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.x, texture.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(texture.data);
    return (textureID);

}

int initBackground(t_scop *scop) {

    GLfloat vertices[] =
    { //     COORDINATES   
        -1.0f, -1.0f,  0.0f,
        1.0f, 1.0f,  0.0f,
        -1.0f, 1.0f,  0.0f,
        1.0f, -1.0f,  0.0f,
    };

    GLuint indices[] =
    {
        0, 1, 2,
        0, 1, 3,
    };

    GLfloat texture[] =
    {
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
    };

    scop->background.VAO = initVertexArray((t_array){vertices, 4});
    initElementArray(scop->background.VAO, (t_array){indices, 6});
    addArrayBuffer(scop->background.VAO, (t_array){texture, 8}, 2 * sizeof(GLfloat), 1);

    if (!(scop->background.programShader = initShaders("shaders/backgroundVS", "shaders/backgroundFS", scop->path)) ||
        !(scop->background.textureID = textureInit(BACKGROUND_IMAGE, scop->path)))
        return (0);
    return (1);
}
