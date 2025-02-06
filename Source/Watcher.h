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

    void Reset()
    {
        *object = copy;
        copy = object;
    }

    bool HasChanged()
    {
        return std::memcmp(&object, &copy, sizeof(T));
    }

    bool HasChanged(T* oldValue)
    {
        if (HasChanged())
        {
            Reset();
            return true;
        }
        return false;
    }

    T Resync()
    {
        Reset();
        return copy;
    }

    T Current()
    {
        return copy;
    }
};
