#pragma once

template<typename T>
class Handle
{
protected:
    T handle;
public:
    Handle()
    {
        handle = T();
    }
    T& getHandle()
    {
        return handle;
    }
};
