#include "scop.h"

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

    GLint status;
    glGetProgramiv(programShader, GL_LINK_STATUS, &status);
    if (!status)
        dprintf(2, "program shader failed to link\n");

    free((void*)vertexShaderSource);
    free((void*)fragmentShaderSource);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    return (programShader);
}