#pragma once
#include "../Interfaces/IHealthable.h"
#include "../../Foundation/GEUtility.h"
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GEContext.h"

typedef void(*HealthCallback)(class HealthComponent* sender, int value);

class HealthComponent : public IHealthable {
private:
    int _hp = 100;
    int _maxHp = 100;

    HealthCallback _onDeath = nullptr;
    HealthCallback _onHurt = nullptr;

public:
    explicit HealthComponent(int hp = 100, int maxHp = 100)
        : _hp(hp), _maxHp(maxHp) {
    }

    // ========== Getters ==========
    int getHP() const override { return _hp; }
    int getMaxHP() const override { return _maxHp; }
    bool isAlive() const override { return _hp>0; }
    float getHpRatio() const { return (_maxHp > 0) ? static_cast<float>(_hp) / _maxHp : 0.0f; }

    // ========== Setters ==========
    void setOnHurt(HealthCallback cb) { _onHurt = cb; }
    void setOnDeath(HealthCallback cb) { _onDeath = cb; }

    // ========== Core Logics ==========
    void takeDamage(int dmg) override {
        if (!isAlive() || dmg <= 0) return;

        _hp = std::max(0, _hp - dmg);
        if (_onHurt) _onHurt(this, dmg);

        if (_hp <= 0) {
            if (_onDeath) _onDeath(this, 0);
        }
    }

    void heal(int value) override {
        if (!isAlive() || value <= 0) return;
        _hp = std::min(_maxHp, _hp + value);
    }

    void reset(int hp, int maxHp) {
        _hp = hp;
        _maxHp = maxHp;
    }
};
