#include "Scope.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"

Scope::Scope()
{
    instance = new Instance(this);
    instance->Create();
}

Scope::~Scope()
{
    instance->Destroy();
    delete instance;
}
