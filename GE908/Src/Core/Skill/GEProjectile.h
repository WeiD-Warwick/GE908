#pragma once
#include <string>
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GECollisible.h"

enum ProjectileOwner {
    FromPlayer,
    FromEnemy
};

class GEProjectile : public GECollisible {
private:
    float _speed;
    float _dirX, _dirY;
    int _damage;
    bool _active;
    ProjectileOwner _owner;

public:
    GEProjectile(const std::string& texturePath, ProjectileOwner owner, int startX, int startY, float dirX, float dirY, float speed, int damage);

    bool isActive() const { return _active; }
    void deactivate() { _active = false; }
    ProjectileOwner getOwner() const { return _owner; }
    int getDamage() const { return _damage; }

    void update(float deltaTime);
};
