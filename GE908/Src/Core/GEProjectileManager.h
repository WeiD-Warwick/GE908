#pragma once
#include "GEPlayer.h"
#include "GEEnemy.h"
#include "../Foundation/GEProvider.h"

#define MAX_PROJECTILES 5000

class GEProjectile : public GECollisible {
private:
    float _speed;
    float _dirX;
    float _dirY;
    int _damage;
    bool _active;
    ProjectileOwner _owner;

public:
    GEProjectile(const std::string& texturePath, ProjectileOwner owner, float centerX, float centerY, float dirX, float dirY, float speed, int damage)
        : GECollisible(texturePath, GECollisionType::Projectile),
        _owner(owner), _speed(speed), _dirX(dirX), _dirY(dirY),
        _damage(damage), _active(true) {

        setCenter(centerX, centerY);
    }

    void deactivate() { _active = false; }
    bool isActive() const { return _active; }
    int getDamage() const { return _damage; }
    ProjectileOwner getOwner() const { return _owner; }

    void update(float deltaTime) {
        if (!_active) return;
        float newCenterX = getCenterX();
        float newCenterY = getCenterY();
        newCenterX += _dirX * _speed * deltaTime;
        newCenterY += _dirY * _speed * deltaTime;

        setCenter(newCenterX, newCenterY);
    }
};

class GEProjectileManager : public ProjectileProvider {

private:

    GEProjectile* _projectiles[MAX_PROJECTILES];
   

public:
    GEProjectileManager();
    ~GEProjectileManager();

    void addProjectile(ProjectileOwner from, float startPointX, float startPointY,
        float dirX, float dirY, float speed, int damage);

    void update(float deltaTime, GEContext& ctx);
    void draw(Window& window, const GECamera& camera);
};