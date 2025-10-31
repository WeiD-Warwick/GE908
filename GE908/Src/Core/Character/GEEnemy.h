#pragma once
#include "BaseCharacter.h"

enum GEEnemyType {
	Normal,
	Fast,
	Heavy,
	StaticShooter
};

class GEEnemy : public BaseCharacter {
private:
	GEEnemyType _type;
	float _attackCooldown = 0.0f;
	float _attackRate = 2.0f;
	bool _isStatic = false;

public:
	GEEnemy(GEEnemyType type);
	~GEEnemy();

	void update(float deltaTime, int playerX, int playerY);
	bool getIsStatic() const { return _isStatic; }
};
