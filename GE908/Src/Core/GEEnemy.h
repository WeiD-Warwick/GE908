#pragma once
#include "GECharacter.h"
#include "../Foundation/GEProvider.h"
#include "../Foundation/GEObjectPool.h"

class GEPlayer;

class GEEnemy : public GECharacter, public GEPoolable {
private:
	GEEnemyType _type;
	float _attackCooldown = 0.0f;
	float _attackRate = 2.0f;
	bool _isStatic = false;

	void bind(GEContext& ctx) override;

	void applyMovementBounds(float& newX, float& newY) override;
public:
	GEEnemy(GEEnemyType type);
	~GEEnemy();

	GEEnemyType getType() const { return _type; }
	void update(float deltaTime, float playerCenterX, float playerCenterY, GEContext& ctx);
	bool getIsStatic() const { return _isStatic; }
	void draw(Window& window, const GECamera& camera) const override;
	void takeDamage(int value) override;

	bool isActiveElement() const override { return isAlive(); }
};
