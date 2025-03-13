#pragma once

#include "Handle.h"
#include "Link.h"

class Engine;
class GLFWwindow;

class Window
    : public Handle<GLFWwindow*>
    , public Link<Engine>
{
public:
    explicit Window(Engine *engine);
    ~Window();

    static void PollEvents();
    [[nodiscard]] bool IsValid() const;
};
