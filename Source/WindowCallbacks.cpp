#include "Window.h"

#include <GLFW/glfw3.h>
#include <cstdio>

UserData& GetUserData(GLFWwindow *window)
{
    auto *ptr = glfwGetWindowUserPointer(window);
    return *static_cast<UserData*>(ptr);
}

void Callback::Error(int err, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

void Callback::Key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Callback::FramebufferSize(GLFWwindow *window, const int width, const int height)
{
    auto &userData = GetUserData(window);
    userData.width = width;
    userData.height = height;
}