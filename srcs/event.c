#include "scop.h"

static void getKeysEvent(t_scop *scop, unsigned char key, char *uniform, char init) {
    static char keyStatus[128];
    static char uniformStatus[128];
    int status;
    
    if (key >= 128)
        return ;
    if (init >= 0) {
        keyStatus[key] = 1;
        uniformStatus[key] = (init != 0);
        return;
    }

    status = glfwGetKey(scop->window, key);
    if (status == GLFW_PRESS && keyStatus[key]) {
        uniformStatus[key] = !uniformStatus[key];
        glUniform1i(glGetUniformLocation(scop->object.programShader, uniform), uniformStatus[key]);
        keyStatus[key] = 0;
    }
    else if (status == GLFW_RELEASE)
        keyStatus[key] = 1;
}

static void getMouseEvent(t_scop *scop) {
    int state;
    double posx, posy;

    state = glfwGetMouseButton(scop->window, GLFW_MOUSE_BUTTON_LEFT);
    glfwGetCursorPos(scop->window, &posx, &posy);
    if (state == GLFW_PRESS)
    {
        t_mat4 matrix = IDENTITY_MAT4;
        t_mat4 tmp;
        float  rotationY;
        
        scop->rotationAngle.y += posx - scop->mouse.x;
        if (fabs(scop->rotationAngle.x + posy - scop->mouse.y) < 90)
            scop->rotationAngle.x += posy - scop->mouse.y;
        rotationY = scop->rotationAngle.y * (PI / 180.0f);
        rotate(matrix, scop->rotationAngle.y, (t_vertex){0.0f, 1.0f, 0.0f}, &tmp);
        rotate(tmp, scop->rotationAngle.x, (t_vertex){cos(rotationY), 0.0f, sin(rotationY)}, &scop->rotation);
    }
    scop->mouse.x = posx;
    scop->mouse.y = posy;
}

static void getMetalEvent(t_scop *scop) {
    static float metalValue = 1.0f;

    if (glfwGetKey(scop->window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        if (metalValue < 0.99f) {
            metalValue += 0.01f;
            glUniform1f(glGetUniformLocation(scop->object.programShader, "metalUni"), metalValue);
        }
    }
    if (glfwGetKey(scop->window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        if (metalValue > 0.01f) {
            metalValue -= 0.01f;
            glUniform1f(glGetUniformLocation(scop->object.programShader, "metalUni"), metalValue);
        }
    }
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

static void moveObject(t_scop *scop) {
    if (glfwGetKey(scop->window, GLFW_KEY_UP) == GLFW_PRESS)
        scop->object.position.z -= 0.1f;
    if (glfwGetKey(scop->window, GLFW_KEY_DOWN) == GLFW_PRESS)
        scop->object.position.z += 0.1f;
    if (glfwGetKey(scop->window, GLFW_KEY_LEFT) == GLFW_PRESS)
        scop->object.position.x -= 0.1f;
    if (glfwGetKey(scop->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        scop->object.position.x += 0.1f;
    if (glfwGetKey(scop->window, GLFW_KEY_SPACE) == GLFW_PRESS)
        scop->object.position.y += 0.1f;
    if (glfwGetKey(scop->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        scop->object.position.y -= 0.1f;
}

static void rotateObject(t_scop *scop) {
    t_mat4 matrix = IDENTITY_MAT4;
    
    if (glfwGetKey(scop->window, GLFW_KEY_Y) == GLFW_PRESS) {
        memcpy(&matrix, &scop->object.rotation, sizeof(t_mat4));
        rotate(matrix, 1.0f, (t_vertex){0.0f, 1.0f, 0.0f}, &scop->object.rotation);
    }
    if (glfwGetKey(scop->window, GLFW_KEY_X) == GLFW_PRESS) {
        memcpy(&matrix, &scop->object.rotation, sizeof(t_mat4));
        rotate(matrix, 1.0f, (t_vertex){1.0f, 0.0f, 0.0f}, &scop->object.rotation);
    }
    if (glfwGetKey(scop->window, GLFW_KEY_Z) == GLFW_PRESS) {
        memcpy(&matrix, &scop->object.rotation, sizeof(t_mat4));
        rotate(matrix, 1.0f, (t_vertex){0.0f, 0.0f, 1.0f}, &scop->object.rotation);
    }
}

void initGetKeysEvent() {
    getKeysEvent(0, GLFW_KEY_N, "activateNormalMap", 1);
    getKeysEvent(0, GLFW_KEY_I, "activateIBL", 1);
    getKeysEvent(0, GLFW_KEY_P, "activatePBR", 1);
    getKeysEvent(0, GLFW_KEY_L, "hasLights", 0);
    getKeysEvent(0, GLFW_KEY_G, "isGray", 0);
}

void getEvents(t_scop *scop) {
    glfwPollEvents();
    getMouseEvent(scop);
    moveObject(scop);
    rotateObject(scop);
    getWindowEvent(scop);
    getKeysEvent(scop, GLFW_KEY_N, "activateNormalMap", -1);
    getKeysEvent(scop, GLFW_KEY_I, "activateIBL", -1);
    getKeysEvent(scop, GLFW_KEY_P, "activatePBR", -1);
    getKeysEvent(scop, GLFW_KEY_L, "hasLights", -1);
    getKeysEvent(scop, GLFW_KEY_G, "isGray", -1);
    getTransitionKeyEvent(scop);
    getMetalEvent(scop);
}