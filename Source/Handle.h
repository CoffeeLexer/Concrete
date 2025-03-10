#pragma once
#include <stdexcept>
#include <vulkan/vulkan_core.h>

template<typename T>
class Handle {
protected:
    T handle = VK_NULL_HANDLE;

    Handle() = default;

public:
    explicit operator T&()
    {
        if (handle == VK_NULL_HANDLE)
            throw std::runtime_error("Handle is NULL");
        return handle;
    }
};

