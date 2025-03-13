#pragma once

#include <source_location>
#include <string_view>

static void panic(const char* message,
    std::source_location location = std::source_location::current());
