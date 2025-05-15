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
