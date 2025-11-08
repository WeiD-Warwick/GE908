#pragma once
#include "../Interfaces/IAttackable.h"

class AttackableComponent : public IAttackable {
public:
    void attack() override {}
};
