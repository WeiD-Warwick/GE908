#include <string>
#include <cmath>
#include "GEEnemy.h"
#include "../Actors/GEProjectileManager.h"

static const std::string EnemyImagePath(GEEnemyType t) {
	switch (t) {
	case Normal:        return "Src/Assets/Textures/enemy_normal.png";
	case Fast:          return "Src/Assets/Textures/enemy_fast.png";
	case Heavy:         return "Src/Assets/Textures/enemy_heavy.png";
	case StaticShooter: return "Src/Assets/Textures/enemy_static.png";
	default:            return "Src/Assets/Textures/enemy_normal.png";
	}
}

void GEEnemy::applyMovementBounds(float& newX, float& newY) {
    float minCenterX = _width / 2.0f;
    float maxCenterX = _mapWidth - _width / 2.0f;
    float minCenterY = _height / 2.0f;
    float maxCenterY = _mapHeight - _height / 2.0f;

    newX = clamp(newX, minCenterX, maxCenterX);
    newY = clamp(newY, minCenterY, maxCenterY);
}

GEEnemy::GEEnemy(GEEnemyType type)
	: BaseCharacter(EnemyImagePath(type), Enemy), _type(type) {
	_width = _image.width;
	_height = _image.height;

	switch (_type) {
    case Normal:
        _hp = 200;_speed = 150;break;
    case Fast:
        _hp = 100;_speed = 270;break;
    case Heavy:
        _hp = 300;_speed = 80;break;
    case StaticShooter:
        _hp = 150;_speed = 0;_isStatic = true;_attackRate = 2.8f;break;
    }
}

GEEnemy::~GEEnemy() {}

void GEEnemy::update(float deltaTime,
    float playerCenterX, float playerCenterY,
    GEProjectileManager& projectileManager)
{
    if (!isAlive()) return;

    const float cx = getCenterX();
    const float cy = getCenterY();

    if (!_isStatic) {
        const float dx = playerCenterX - cx;
        const float dy = playerCenterY - cy;

        if (dx != 0.0f || dy != 0.0f) {
            moveUpdate(deltaTime, dx, dy);
        }
        return;
    }
    else {
        _attackCooldown += deltaTime;
        if (_attackCooldown < _attackRate) return;
        _attackCooldown = 0.0f;

        float vx = playerCenterX - cx;
        float vy = playerCenterY - cy;
        const float len = std::sqrt(vx * vx + vy * vy);
        if (len == 0.0f) return;

        vx /= len;
        vy /= len;

        projectileManager.addProjectile(FromEnemy, cx, cy, vx, vy, 100.0f, 200);
    }
}
