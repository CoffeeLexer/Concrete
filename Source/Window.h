#pragma once

class Engine;
class GLFWwindow;

class Window
{
    const Engine &engine;
    GLFWwindow *window;

public:

    Window(const Engine &engine);
    ~Window();

    void SwapBuffers();
    void PollEvents();
    bool IsValid();
};