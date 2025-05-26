#include "Scope.h"

#include "Instance.h"
#include "Window.h"
#include "Device.h"
#include "Shader.h"

Scope::Scope()
    : instance(*this)
    , window(*this)
    , device(*this)
    , shaderManager(*this)
    , backbuffer(*this)
{}

Scope::~Scope() {}

void Scope::pollEvents()
{
    Window::PollEvents();
}

bool Scope::isActive() {
    return window.IsValid();
}

void Scope::beginFrame() {
    backbuffer.beginFrame();
}

void Scope::draw(Shader &shader) {
    backbuffer.draw(shader);
}

void Scope::endFrame() {
    backbuffer.endFrame();
}

void Scope::waitIdle() {
    backbuffer.waitIdle();
}
