#pragma once

#include <concepts>
#include <functional>
#include <type_traits>

class Engine;

class Owned
{
public:
    Owned() = default;
    virtual ~Owned() = default;
    virtual Engine& Owner() = 0;
};

template <typename Class, typename Result, typename... Args>
auto result_type(Result (Class::*)(Args...)) -> Result;

template <typename T>
concept IsObject = requires () {
    // Function exists
    std::is_member_pointer_v<decltype(&T::Create)>;
    std::is_member_pointer_v<decltype(&T::Destroy)>;
    // Function return type
    requires std::same_as<decltype(result_type(&T::Create)), void>;
    requires std::same_as<decltype(result_type(&T::Destroy)), void>;
    // Member variables
    // { &T::Stump } -> std::same_as<int T::*>;
    requires std::is_base_of_v<Owned, T>;
};

template<IsObject T>
class Handler : public T
{
    std::function<Engine&()> fn;
public:
    explicit Handler(Engine& engine);
    Engine& Owner() override;
};
