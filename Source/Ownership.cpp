#include "Ownership.h"

std::function<Engine&()> Wrap(Engine& engine) {
    return [&engine] -> Engine& {
        return engine;
    };
}

template<IsObject T>
Handler<T>::Handler(Engine& engine)
   : fn(Wrap(engine))
{}
template<IsObject T>
Engine& Handler<T>::Owner() override
{
    return fn();
}
