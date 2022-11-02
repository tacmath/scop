#include "scop.h"

void getNormalKeyEvent(t_scop *scop) {
    static char keyStatusN = 1;
    static char normalMap = 1;
    int status;

    status = glfwGetKey(scop->window, GLFW_KEY_N);
    if (status == GLFW_PRESS && keyStatusN) {
        normalMap = !normalMap;
        glUniform1i(glGetUniformLocation(scop->object.programShader, "activateNormalMap"), normalMap);
        keyStatusN = 0;
    }
    else if (status == GLFW_RELEASE)
        keyStatusN = 1;
}

void getLightKeyEvent(t_scop *scop) {
    static char keyStatus = 1;
    static char light = 0;
    int status;

    status = glfwGetKey(scop->window, GLFW_KEY_L);
    if (status == GLFW_PRESS && keyStatus) {
        light = !light;
        glUniform1i(glGetUniformLocation(scop->object.programShader, "hasLights"), light);
        keyStatus = 0;
    }
    else if (status == GLFW_RELEASE)
        keyStatus = 1;
}

void getPBRKeyEvent(t_scop *scop) {
    static char keyStatus = 1;
    static char pbr = 1;
    int status;

    status = glfwGetKey(scop->window, GLFW_KEY_P);
    if (status == GLFW_PRESS && keyStatus) {
        pbr = !pbr;
        glUniform1i(glGetUniformLocation(scop->object.programShader, "activatePBR"), pbr);
        keyStatus = 0;
    }
    else if (status == GLFW_RELEASE)
        keyStatus = 1;
}

void getIBLKeyEvent(t_scop *scop) {
    static char keyStatus = 1;
    static char ibl = 1;
    int status;

    status = glfwGetKey(scop->window, GLFW_KEY_I);
    if (status == GLFW_PRESS && keyStatus) {
        ibl = !ibl;
        glUniform1i(glGetUniformLocation(scop->object.programShader, "activateIBL"), ibl);
        keyStatus = 0;
    }
    else if (status == GLFW_RELEASE)
        keyStatus = 1;
}

void getMouseEvent(t_scop *scop) {
    int state;
    double posx, posy;

    state = glfwGetMouseButton(scop->window, GLFW_MOUSE_BUTTON_LEFT);
    glfwGetCursorPos(scop->window, &posx, &posy);
    if (state == GLFW_PRESS)
    {
        t_mat4 matrix = IDENTITY_MAT4;
        t_mat4 tmp;
        float  rotationY;
        
        scop->object.rotation.y += posx - scop->mouse.x;
        if (fabs(scop->object.rotation.x + posy - scop->mouse.y) < 90)
            scop->object.rotation.x += posy - scop->mouse.y;
        rotationY = scop->object.rotation.y * (PI / 180.0f);
        rotate(matrix, scop->object.rotation.y, (t_vertex){0.0f, 1.0f, 0.0f}, &tmp);
        rotate(tmp, scop->object.rotation.x, (t_vertex){cos(rotationY), 0.0f, sin(rotationY)}, &scop->rotation);
    }
    scop->mouse.x = posx;
    scop->mouse.y = posy;
}

void getTransitionKeyEvent(t_scop *scop) {
    static char transition = -1;
    static char keyStatusT = 1;
    int status;

    status = glfwGetKey(scop->window, GLFW_KEY_T);
    if (status == GLFW_PRESS && keyStatusT) {
        transition = -transition;
        keyStatusT = 0;
    }
    else if (status == GLFW_RELEASE)
        keyStatusT = 1;
    if (scop->transition >= 0 && transition == -1) {
        scop->transition -= 0.02;
        glUniform1f(glGetUniformLocation(scop->object.programShader, "transition"), scop->transition);
    }
    else if (scop->transition <= 1 && transition == 1) {
        scop->transition += 0.02;
        glUniform1f(glGetUniformLocation(scop->object.programShader, "transition"), scop->transition);
    }
}

void getWindowEvent(t_scop *scop) {
    static int oldHeight = 0;
    static int oldWidth = 0;
    int width, height;

    glfwGetWindowSize(scop->window, &width, &height);
    if (width != oldWidth || height != oldHeight) {
        perspective(45.0f, (float)width / (float)height, 0.1f, 200.0f, &scop->projection);
        oldWidth = width;
        oldHeight = height;
    }
}

void getEvents(t_scop *scop) {
    glfwPollEvents();
    getMouseEvent(scop);
    getWindowEvent(scop);
    getNormalKeyEvent(scop);
    getPBRKeyEvent(scop);
    getIBLKeyEvent(scop);
    getLightKeyEvent(scop);
    getTransitionKeyEvent(scop);
}