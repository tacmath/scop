#include "scop.h"

float cameraPosZ = -2.0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        cameraPosZ += 0.5f;
    else if (yoffset < 0)
        cameraPosZ -= 0.5f;
}

int initWindow(t_scop *scop) {
    if( !glfwInit() )
        return (0);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // On veut OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux ; ne devrait pas être nécessaire
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
    if(!(scop->window = glfwCreateWindow( 400, 300, "Scop", NULL, NULL))) {
        glfwTerminate();
        return (0);
    }
    glfwMakeContextCurrent(scop->window);
//    glViewport(0, 0, 400, 300);
    if (glewInit() != GLEW_OK)
        return (0);
//    scop->monitor = glfwGetWindowMonitor(scop->window);	
    glfwSetInputMode(scop->window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetScrollCallback(scop->window, scroll_callback);
    return (1);
}

GLuint initShaders(char *vertexShaderFile, char *fragmentShaderFile) {
    const char* vertexShaderSource = getShaderSource(vertexShaderFile);
    const char* fragmentShaderSource = getShaderSource(fragmentShaderFile);
  //  printf("\n %s \n%s\n",vertexShaderSource, fragmentShaderSource);
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