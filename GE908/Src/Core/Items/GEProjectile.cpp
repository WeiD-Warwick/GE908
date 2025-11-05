#include "GEProjectile.h"

GEProjectile::GEProjectile(const std::string& texturePath, ProjectileOwner owner, float startPointX, float startPointY, float dirX, float dirY, float speed, int damage)
    : GECollisible(texturePath, Projectile),
    _owner(owner), _speed(speed), _dirX(dirX), _dirY(dirY),
    _damage(damage), _active(true) {

    setCenter(startPointX, startPointY);
}

void GEProjectile::update(float deltaTime) {
    if (!_active) return;
    float newCenterX = getCenterX();
    float newCenterY = getCenterY();
    newCenterX += _dirX * _speed * deltaTime;
    newCenterY += _dirY * _speed * deltaTime;

    setCenter(newCenterX, newCenterY);
}