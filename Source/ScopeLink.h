#pragma once

class Scope;
class ScopeLink
{
    Scope *scope = nullptr;
public:
    ScopeLink(Scope *ptr)
    {
        scope = ptr;
    }
    [[nodiscard]] Scope& operator()()
    {
        return *scope;
    }
};