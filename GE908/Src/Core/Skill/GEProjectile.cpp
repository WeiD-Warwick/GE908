#include "GEProjectile.h"

GEProjectile::GEProjectile(const std::string& texturePath, ProjectileOwner owner, int startX, int startY, float dirX, float dirY, float speed, int damage)
    : GECollisible(texturePath, Projectile),
    _owner(owner), _speed(speed), _dirX(dirX), _dirY(dirY),
    _damage(damage), _active(true)
{

    setPosition(startX, startY);
}

void GEProjectile::update(float deltaTime) {
    if (!_active) return;
    _originX += static_cast<int>(_dirX * _speed * deltaTime);
    _originY += static_cast<int>(_dirY * _speed * deltaTime);
}