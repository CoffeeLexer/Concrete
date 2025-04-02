#include "Scope.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"

Scope::Scope()
{
    instance = new Instance;
    static_cast<Scoped&>(*instance).setScope(this);


    window = new Window;
    device = new Device;
    window->SetScope(this);
    device->SetScope(this);
    window->Create();
    device->Create();
}
