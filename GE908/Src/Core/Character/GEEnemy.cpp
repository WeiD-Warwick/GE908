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
	: BaseCharacter(EnemyImagePath(type), None), _type(type)
{
	switch (_type) {
	case Normal:
		_hp = 100; _speed = 80; break;
	case Fast:
		_hp = 60; _speed = 80; break;
	case Heavy:
		_hp = 200; _speed = 80; break;
	case StaticShooter:
		_hp = 80; _speed = 0; _isStatic = true; _attackRate = 3.0f; break;
	}
	_width = image.width;
	_height = image.height;
}

GEEnemy::~GEEnemy() {}

void GEEnemy::update(float dt, int characterX, int characterY) {
	if (!isAlive()) return;

	if (!_isStatic) {
		int currentX = getX();
		int currentY = getY();

		int dirX = (characterX > currentX) - (characterX < currentX);
		int dirY = (characterY > currentY) - (characterY < currentY);

		float step = _speed * dt;
		_accumX += dirX * step;
		_accumY += dirY * step;

		// Debouncer
		int moveX = (int)std::floor(std::abs(_accumX)) * ((_accumX >= 0) ? 1 : -1);
		int moveY = (int)std::floor(std::abs(_accumY)) * ((_accumY >= 0) ? 1 : -1);
		_accumX -= moveX;
		_accumY -= moveY;

		setPosition(currentX + moveX, currentY + moveY);
	}
	else {
		_attackCooldown += dt;
		if (_attackCooldown > _attackRate) {
			_attackCooldown = 0.f;
			GELog::shared().info("Static enemy fired projectile");
		}
	}
}
