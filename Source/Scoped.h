#pragma once

class Scope;
class Scoped
{
    friend class Scope;
    Scope *scope = nullptr;
    void setScope(Scope *scope)
    {
        this->scope = scope;
    }
public:
    Scoped() = default;
    [[nodiscard]] Scope& getScope() const
    {
        return *scope;
    }
};