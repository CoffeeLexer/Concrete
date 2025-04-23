#pragma once

class Scope;
class ScopeLink
{
    Scope &scope;
public:
    ScopeLink(Scope &scope) : scope(scope) {}
    [[nodiscard]] Scope& operator()()
    {
        return scope;
    }
};