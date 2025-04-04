#pragma once

template<typename T>
class Handle
{
protected:
    T handle = T();
public:
    Handle() = default;
    T getHandle()
    {
        return handle;
    }
};
