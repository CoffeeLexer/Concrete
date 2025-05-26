#pragma once

#include "Instance.h"
#include "Window.h"
#include "Device.h"
#include "ShaderManager.h"
#include "Backbuffer.h"

class Shader;
class Scope
{
    Instance instance;
    Window window;
    Device device;
    ShaderManager shaderManager;
    Backbuffer backbuffer;
public:
    Scope();
    ~Scope();

    bool isActive();
    void pollEvents();
    void beginFrame();
    void draw(Shader &shader);
    void endFrame();
    void waitIdle();

    Instance& getInstance() { return instance; }
    Window& getWindow() { return window; }
    ShaderManager& getShaderManager() { return shaderManager; }
    Device& getDevice() { return device; }
    Backbuffer& getBackbuffer() { return backbuffer; }
};