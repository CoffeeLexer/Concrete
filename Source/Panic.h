#pragma once

#include <source_location>
#include <vulkan/vulkan.h>

void panic(const char* message,
    std::source_location location = std::source_location::current());

template <typename ...Args>
void critical(const char* msg, VkResult (*fn)(Args...), Args &&... args);