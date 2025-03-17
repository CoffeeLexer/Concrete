#pragma once

#include <source_location>

void panic(const char* message,
    std::source_location location = std::source_location::current());
