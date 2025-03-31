#pragma once

#include "Handle.h"
#include "Ownership.h"

class Engine;
class GLFWwindow;
struct UserData;

class Window
    : public Handle<GLFWwindow*>
    , public Owned
{
    UserData *userData = nullptr;
public:
    explicit Window() = default;
    ~Window() = default;

    void Create();
    void Destroy();

    static void PollEvents();
    [[nodiscard]] bool IsValid() const;
};
