#include "Scope.h"

#include "Instance.h"
#include "Window.h"
#include "Device.h"

Scope::Scope()
    : instance(*this)
    , window(*this)
    , device(*this)
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

void Scope::draw() {
    backbuffer.draw();
}
