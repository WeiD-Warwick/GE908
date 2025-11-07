#pragma once
#include "BaseCharacter.h"

class GEProjectileManager;

class GEPlayer;

enum class GEEnemyType {
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

	void applyMovementBounds(float& newX, float& newY) override;

public:
	GEEnemy(GEEnemyType type);
	~GEEnemy();

	void update(float deltaTime, float playerCenterX, float playerCenterY, GEProjectileManager& projectileManager);
	bool getIsStatic() const { return _isStatic;}
};
