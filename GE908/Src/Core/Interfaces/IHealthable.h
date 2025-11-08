#pragma once

class IHealthable {
public:
    virtual void takeDamage(int dmg) = 0;
    virtual void heal(int value) = 0;
    virtual int  getHP() const = 0;
    virtual int  getMaxHP() const = 0;
    virtual bool isAlive() const = 0;
    virtual ~IHealthable() = default;
};
