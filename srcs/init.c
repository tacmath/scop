#include "scop.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

int initWindow(t_scop *scop) {
    if(!glfwInit()) {
        dprintf(2, "Glfw failed to init\n");
        return (0);
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
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

GLuint initShaders(char *vertexShaderFile, char *fragmentShaderFile) {
    const char* vertexShaderSource = getShaderSource(vertexShaderFile);
    const char* fragmentShaderSource = getShaderSource(fragmentShaderFile);
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

GLuint initVertexArray(t_array vertices, t_array indices) {
    GLuint VBO, EBO, VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size * sizeof(t_vertex), vertices.data, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size * sizeof(GLuint), indices.data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return (VAO);
}

int textureInit(t_scop *scop, char *fileName) {
    t_texture texture;

    stbi_set_flip_vertically_on_load(1);
    if (!(texture.data = stbi_load(fileName, &texture.x, &texture.y, &texture.numColCh, 0))) {
        dprintf(2, "Failed to load %s\n", fileName);
        return (0);
    }
    glGenTextures(1, &scop->textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scop->textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.x, texture.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(texture.data);
    return (1);

}
