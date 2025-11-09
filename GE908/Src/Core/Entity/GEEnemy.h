#pragma once
#include "GECharacter.h"
#include "../Interface/GEProvider.h"
#include "../../Foundation/GEObjectPool.h"
#include "../SaveLoad/GECodable.h"

class GEPlayer;

class GEEnemy : public GECharacter, public GEPoolable, public GECodable<GEEnemyState> {
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

	float getAttackCooldown() const { return _attackCooldown; }
	void setAttackCooldown(float value) { _attackCooldown = value; }

	bool isActiveElement() const override { return isAlive(); }

	GEEnemyState snapshotState() const override;
	void applyState(const GEEnemyState& state) override;
};
