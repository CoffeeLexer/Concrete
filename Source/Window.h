#pragma once

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"

class Engine;
class GLFWwindow;

class Window : public Handle<GLFWwindow*>
{
    Scope &scope;
    void *userData = nullptr;
public:
    explicit Window(Scope &scope);
    ~Window();

    static void PollEvents();
    [[nodiscard]] bool IsValid() const;
};
