#include <string>
#include <cmath>
#include "GEEnemy.h"
#include "GEProjectileManager.h"

static const std::string EnemyImagePath(GEEnemyType t) {
	switch (t) {
    case GEEnemyType::Normal:        return "Src/Assets/Textures/enemy_normal.png";
    case GEEnemyType::Fast:          return "Src/Assets/Textures/enemy_fast.png";
    case GEEnemyType::Heavy:         return "Src/Assets/Textures/enemy_heavy.png";
    case GEEnemyType::StaticShooter: return "Src/Assets/Textures/enemy_static.png";
	default:                         return "Src/Assets/Textures/enemy_normal.png";
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
    : Character(EnemyImagePath(type), GECollisionType::Enemy), _type(type) {
    _width = _image.width;
    _height = _image.height;
    setContactDamageCooldownDuration(0.5f);

	switch (_type) {
    case GEEnemyType::Normal:
        _hp = 200;
        _speed = 150;
        _maxHp = _hp;
        break;
    case GEEnemyType::Fast:
        _hp = 100;
        _speed = 270;
        _maxHp = _hp;
        break;
    case GEEnemyType::Heavy:
        _hp = 300;
        _speed = 80;
        _maxHp = _hp;
        break;
    case GEEnemyType::StaticShooter:
        _hp = 150;
        _speed = 0;
        _maxHp = _hp;
        _isStatic = true;
        _attackRate = 2.8f;
        break;
    }
}

GEEnemy::~GEEnemy() = default;

void GEEnemy::update(float deltaTime,
    float playerCenterX, float playerCenterY,
    GEProjectileManager& projectileManager) {
    updateCharacterState(deltaTime);

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

        projectileManager.addProjectile(ProjectileOwner::FromEnemy, cx, cy, vx, vy, 100.0f, 200);
    }
}

void GEEnemy::draw(Window& window, const GECamera& camera) const {
    Character::draw(window, camera);
}

void GEEnemy::takeDamage(int value) {
    if (value <= 0) return;
    Character::takeDamage(value);
    triggerDamageFlash(GEColor(255, 255, 255), 0.25f);
}