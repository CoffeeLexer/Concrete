#include "Panic.h"

#include <sstream>

void panic(const char* message, const std::source_location location)
{
    std::stringstream ss;
    ss << location.file_name() << '('
       << location.line() << ':'
       << location.column() << ") `"
       << location.function_name() << "`: "
       << message << '\n';
    throw std::runtime_error(ss.str());
}

template <typename ...Args>
void critical(const char* msg, VkResult (*fn)(Args...), Args &&... args)
{
    if (VK_SUCCESS != (*fn)(std::forward<Args>(args)...))
        panic(msg);
}