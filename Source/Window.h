#pragma once

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"

class Engine;
class GLFWwindow;

class Window : public Handle<GLFWwindow*>
{
    friend Scope::Scope(), Scope::~Scope();
    ScopeLink scope;
    void *userData = nullptr;

    explicit Window(Scope *scope);

    void Create();
    void Destroy();
public:


    static void PollEvents();
    [[nodiscard]] bool IsValid() const;
};
