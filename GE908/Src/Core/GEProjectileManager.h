#pragma once
#include "GEPlayer.h"
#include "GEEnemy.h"
#include "../Foundation/GEProvider.h"
#include "../Foundation/GEObjectPool.h"

#define MAX_PROJECTILES 5000

// ================== Projectile ==================
class GEProjectile : public GECollisible, public GEPoolable {
private:
    float _speed = 0.0f;
    float _dirX = 0.0f;
    float _dirY = 0.0f;
    int _damage = 0;
    bool _active = false;
    ProjectileOwner _owner;

public:
    GEProjectile() = default;

    GEProjectile(const std::string& texturePath, ProjectileOwner owner,
        float centerX, float centerY, float dirX, float dirY,
        float speed, int damage)
        : GECollisible(texturePath, GECollisionType::Projectile),
        _owner(owner), _speed(speed), _dirX(dirX), _dirY(dirY),
        _damage(damage), _active(true) {
        setCenter(centerX, centerY);
    }

    bool isActiveElement() const override { return _active; }
    void deactivate() { _active = false; }

    ProjectileOwner getOwner() const { return _owner; }
    int getDamage() const { return _damage; }

    void spawn(const std::string& texturePath, ProjectileOwner owner,
        float centerX, float centerY, float dirX, float dirY,
        float speed, int damage) {
        _image.load(texturePath);
        _owner = owner;
        _speed = speed;
        _dirX = dirX;
        _dirY = dirY;
        _damage = damage;
        _active = true;
        setCenter(centerX, centerY);
    }

    void update(float deltaTime) {
        if (!_active) return;
        float newCenterX = getCenterX() + _dirX * _speed * deltaTime;
        float newCenterY = getCenterY() + _dirY * _speed * deltaTime;
        setCenter(newCenterX, newCenterY);
    }
};

// ================== Projectile Manager ==================
class GEProjectileManager : public ProjectileProvider {
private:
    GEObjectPool<GEProjectile*> _projectiles;

public:
    GEProjectileManager();
    ~GEProjectileManager();

    void addProjectile(ProjectileOwner from, float startPointX, float startPointY,
        float dirX, float dirY, float speed, int damage);

    void update(float deltaTime, GEContext& ctx);
    void draw(Window& window, const GECamera& camera);
};
