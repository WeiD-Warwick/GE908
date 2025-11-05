#pragma once
#include "BaseCharacter.h"
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GESaveData.h"
#include "../Items/GEProjectile.h"

#define MAX_PROJECTILES 20

class GEMapsManager;

class GEEnemyManager;

class GEProjectileManager;

class GEPlayer : public BaseCharacter {
private:
	const GESaveData* _saveData = nullptr;
	const GEMapsManager* _mapsManager = nullptr;
	const GEEnemyManager* _enemyManager = nullptr;

	float _attackTimer = 0.0f;
	float _attackIntervalBase = 3.0f;
	float _attackSpeedMultiplier = 1.0f;

	GEProjectile* _projectiles[MAX_PROJECTILES];
	int _projectileCount = 0;

	bool collidesWithWater(int newX, int newY, const GEMapsManager& mapsManager) const;

	bool collidesWithEnemies(int newX, int newY, const GEEnemyManager& enemyManager) const;

	bool isBlockedAt(float x, float y) const;

public:

	GEPlayer();
	~GEPlayer();

	void bindWorldContext(const GEMapsManager* maps, const GEEnemyManager* enemies);
	
	void update(float deltaTime, GEWindow& window);

	void updateAttack(float deltaTime, const GEEnemyManager& enemyManager, GEProjectileManager& projectileManager);

};
