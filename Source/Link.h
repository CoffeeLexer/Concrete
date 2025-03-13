#pragma once

#include "Panic.h"

template<typename T>
class Link
{
protected:
    T *owner;
public:
    explicit Link(T *owner) : owner(owner) {}
    T& Owner()
    {
        if (owner == nullptr)
            panic("handle is null");

        return *owner;
    }
};
