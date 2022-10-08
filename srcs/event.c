#include "scop.h"

void getEvents(t_scop *scop) {
    int state;
    double posx, posy;
    static char transition = -1;
    static char keyStatusT = 1;
    
    glfwPollEvents();
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
    if (glfwGetKey(scop->window, GLFW_KEY_T) == GLFW_PRESS && keyStatusT) {
        transition = -transition;
        keyStatusT = 0;
    }
    else if (glfwGetKey(scop->window, GLFW_KEY_T) == GLFW_RELEASE)
        keyStatusT = 1;
    if (scop->transition >= 0 && transition == -1)
        scop->transition -= 0.02;
    else if (scop->transition <= 1 && transition == 1)
        scop->transition += 0.02;
    scop->mouse.x = posx;
    scop->mouse.y = posy;
}