#pragma once
#include "Panic.h"

template<typename T>
class Handle
{
protected:
    T handle;
public:
    Handle()
    {
        handle = nullptr;
    }
    operator T()
    {
        if (handle == nullptr)
            panic("handle is null");
        return handle;
    }
};
