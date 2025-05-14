#pragma once

#include "ScopeLink.h"
#include "Scope.h"

class Object
{
    friend Scope::Scope(), Scope::~Scope();
    virtual void Create() = 0;
    virtual void Destroy() = 0;
    virtual ~Object() = default;
protected:
    ScopeLink scope;
public:
    Object(Scope &scope) : scope(scope) {}
};
