#include <string>
#include "GEEnemy.h"
#include "../Items/GEProjectileManager.h"
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
	: BaseCharacter(EnemyImagePath(type), Enemy), _type(type) {
	_width = _image.width;
	_height = _image.height;

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

}

GEEnemy::~GEEnemy() {}

void GEEnemy::update(float deltaTime, float playerCenterX, float playerCenterY, GEProjectileManager& projectileManager) {
	if (!isAlive()) return;

	float currentCenterX = getCenterX();
	float currentCenterY = getCenterY();

	if (!_isStatic) {
		float dirX = (playerCenterX > currentCenterX) - (playerCenterX < currentCenterX);
		float dirY = (playerCenterY > currentCenterY) - (playerCenterY < currentCenterY);

		float moveDelta = _speed * deltaTime;
		float moveAmount = moveDelta > 1.0f ? moveDelta : 1.0f;

		_accumX += dirX * moveDelta;
		_accumY += dirY * moveDelta;

		float moveX = std::floor(std::abs(_accumX)) * ((_accumX >= 0) ? 1 : -1);
		float moveY = std::floor(std::abs(_accumY)) * ((_accumY >= 0) ? 1 : -1);
		_accumX -= moveX;
		_accumY -= moveY;

		setCenter(currentCenterX + moveX, currentCenterY + moveY);
	}
	else {
		_attackCooldown += deltaTime;
		if (_attackCooldown < _attackRate) return;
		_attackCooldown = 0.0f;

		float targetX = playerCenterX;
		float targetY = playerCenterY;
		float dirX = targetX - currentCenterX;
		float dirY = targetY - currentCenterY;
		float len = sqrtf(dirX * dirX + dirY * dirY);

		if (len == 0.0f) return;

		dirX /= len;
		dirY /= len;

		projectileManager.addProjectile(FromEnemy, currentCenterX, currentCenterY, dirX, dirY, 100.0f, 200);
	}
}
