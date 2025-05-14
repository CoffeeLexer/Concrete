#include "Scope.h"

#include "Instance.h"
#include "Window.h"
#include "Device.h"

Scope::Scope()
{
    instance = new Instance(*this);
    window = new Window(*this);
    device = new Device(*this);
}

Scope::~Scope()
{
    delete device;
    delete window;
    delete instance;
}
