#include "scop.h"

void getEvents(t_scop *scop) {
     int state;
     double posx, posy;

    
    glfwPollEvents();
    state = glfwGetMouseButton(scop->window, GLFW_MOUSE_BUTTON_LEFT);
    glfwGetCursorPos(scop->window, &posx, &posy);
    if (state == GLFW_PRESS)
    {
        scop->object.rotation.y += posx - scop->mouse.x;
        if (fabs(scop->object.rotation.x + posy - scop->mouse.y) < 90)
            scop->object.rotation.x += posy - scop->mouse.y;
    }
    scop->mouse.x = posx;
    scop->mouse.y = posy;
}