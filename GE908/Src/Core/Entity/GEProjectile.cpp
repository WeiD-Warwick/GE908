#include "GEProjectile.h"

GEProjectileState GEProjectile::snapshotState() const {
    GEProjectileState state;
    state.owner = _owner;
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.dirX = _dirX;
    state.dirY = _dirY;
    state.speed = _speed;
    state.damage = _damage;
    state.activate();
    return state;
}

void GEProjectile::applyState(const GEProjectileState& state) {
    _owner = state.owner;
    _speed = state.speed;
    _dirX = state.dirX;
    _dirY = state.dirY;
    _damage = state.damage;
    _active = true;

    const char* texturePath = (state.owner == ProjectileOwner::FromPlayer)
        ? Projectile::PLAYER_PROJECTILE_TEXTURE
        : Projectile::ENEMY_PROJECTILE_TEXTURE;
    _image.load(texturePath);
    setCenter(state.centerX, state.centerY);
}