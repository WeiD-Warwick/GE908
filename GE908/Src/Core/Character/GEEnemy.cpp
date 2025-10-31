#include <string>
#include "GEEnemy.h"
#include "../../Foundation/GELog.h"

static const std::string EnemyImagePath(GEEnemyType t) {
	switch (t) {
	case Normal:        return "Src/Assets/Textures/enemy_normal.png";
	case Fast:          return "Src/Assets/Textures/enemy_fast.png";
	case Heavy:         return "Src/Assets/Textures/enemy_heavy.png";
	case StaticShooter: return "Src/Assets/Textures/enemy_static.png";
	default:            return "Src/Assets/Textures/enemy_normal.png";
	}
}

GEEnemy::GEEnemy(GEEnemyType type)
	: BaseCharacter(0, 0, EnemyImagePath(type), None), _type(type)
{
	switch (_type) {
	case Normal:
		_hp = 100; _speed = 20; break;
	case Fast:
		_hp = 60; _speed = 80; break;
	case Heavy:
		_hp = 200; _speed = 30; break;
	case StaticShooter:
		_hp = 80; _speed = 0; _isStatic = true; _attackRate = 3.0f; break;
	}
	_width = image.width;
	_height = image.height;
}

GEEnemy::~GEEnemy() {}

void GEEnemy::update(float deltaTime, int playerX, int playerY) {
	if (!isAlive) return;

	if (!_isStatic) {
		int dirX = (playerX > _originX) - (playerX < _originX);
		int dirY = (playerY > _originY) - (playerY < _originY);

		float moveDelta = _speed * deltaTime;
		_originX += static_cast<int>(dirX * moveDelta);
		_originY += static_cast<int>(dirY * moveDelta);
	}
	else {
		_attackCooldown += deltaTime;
		if (_attackCooldown > _attackRate) {
			_attackCooldown = 0;
			GELog::shared().info("Static enemy fired projectile");
		}
	}
}