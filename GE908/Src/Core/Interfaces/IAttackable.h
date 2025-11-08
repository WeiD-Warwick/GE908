#pragma once

class IAttackable {
public:
    virtual void attack() = 0;
    virtual ~IAttackable() = default;
};