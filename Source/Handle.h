#pragma once

template<typename T>
class Handle
{
protected:
    T &ref;
public:
    explicit Handle(T &reference) : ref(reference) {}
    const T& operator()()
    {
        return ref;
    }
};
