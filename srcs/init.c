#include "scop.h"

char* glStringEnum(GLenum glenum)
{
    char* enumString = "NONE";

    switch (glenum)
    {
    case GL_INVALID_ENUM:                   enumString = "INVALID_ENUM"; break;
    case GL_INVALID_VALUE:                  enumString = "INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:              enumString = "INVALID_OPERATION"; break;
    case GL_OUT_OF_MEMORY:                  enumString = "OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:  enumString = "INVALID_FRAMEBUFFER_OPERATION"; break;

    case GL_DEBUG_SEVERITY_MEDIUM:          enumString = "GL_DEBUG_SEVERITY_MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH:            enumString = "GL_DEBUG_SEVERITY_HIGH"; break;
    case GL_DEBUG_SEVERITY_LOW:             enumString = "GL_DEBUG_SEVERITY_LOW"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:    enumString = "GL_DEBUG_SEVERITY_NOTIFICATION"; break;

    case GL_DEBUG_SOURCE_API:               enumString = "GL_DEBUG_SOURCE_API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     enumString = "GL_DEBUG_SOURCE_WINDOW_SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:   enumString = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:       enumString = "GL_DEBUG_SOURCE_THIRD_PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION:       enumString = "GL_DEBUG_SOURCE_APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER:             enumString = "GL_DEBUG_SOURCE_OTHER"; break;

    case GL_DEBUG_TYPE_ERROR:               enumString = "GL_DEBUG_TYPE_ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: enumString = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  enumString = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         enumString = "GL_DEBUG_TYPE_PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         enumString = "GL_DEBUG_TYPE_PERFORMANCE"; break;
    case GL_DEBUG_TYPE_MARKER:              enumString = "GL_DEBUG_TYPE_MARKER"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          enumString = "GL_DEBUG_TYPE_PUSH_GROUP"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           enumString = "GL_DEBUG_TYPE_POP_GROUP"; break;
    case GL_DEBUG_TYPE_OTHER:               enumString = "GL_DEBUG_TYPE_OTHER"; break;
    }
    return enumString;
}

static void glfwCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static openGL_Callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    (void)id;
    (void)userParam;
    (void)length;
    if (type != GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR && (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH || type == GL_DEBUG_TYPE_ERROR))
        fprintf(stderr, "Opengl :  severity : %s  type : %s  source : %s\nmessage : %s\n\n", glStringEnum(severity), glStringEnum(type), glStringEnum(source), message);
}

static void debug(void) {
    printf("%s\n", glGetString(GL_VERSION));
    glfwSetErrorCallback(&glfwCallback);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(&openGL_Callback, 0);
}

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
    debug();
//    printf("%s\n", glGetString(GL_VERSION));
    return (1);
}

static void compileErrors(unsigned int shader, const char* type, char* ShaderSource)
{
    // Stores status of compilation
    GLint hasCompiled;
    // Character array to store error message in
    char infoLog[1024];
    if (!strcmp(type, "PROGRAM"))
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            dprintf(2, "SHADER_LINKING_ERROR for:%s\n", infoLog);
        }
        return;
    }
    glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
    if (hasCompiled == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        dprintf(2, "SHADER_COMPILATION_ERROR %s for:%s\n", ShaderSource, infoLog);
    }
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
    compileErrors(vertexShader, "VERTEX", vertexShaderSource);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
    compileErrors(vertexShader, "FRAGMENT", fragmentShaderSource);

	GLuint programShader = glCreateProgram();
	glAttachShader(programShader, vertexShader);
	glAttachShader(programShader, fragmentShader);
	glLinkProgram(programShader);
    compileErrors(vertexShader, "PROGRAM", 0);

    free((void*)vertexShaderSource);
    free((void*)fragmentShaderSource);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    return (programShader);
}