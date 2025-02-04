#pragma once
#include <cstring>

template<typename T>
class Watcher
{
    T& object;
    T copy;
public:
    Watcher(T& obj)
    : object(obj)
    {
        copy = object;
    }
    bool hasUpdated()
    {
        return std::memcmp(&object, &copy, sizeof(T));
    }
    bool hasUpdated(T* oldValue)
    {
        if (hasUpdated())
        {
            *oldValue = copy;
            copy = object;
            return true;
        }
        return false;
    }
};
