#pragma once
#include "BaseCharacter.h"
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GESaveData.h"
#include "../Skill/GEProjectile.h"

#define MAX_PROJECTILES 20

class GEMapsManager;

class GEEnemyManager;

class GEProjectileManager;

class GEPlayer : public BaseCharacter {
private:
	GESaveData* _saveData = nullptr;

	float _attackTimer = 0.0f;
	float _attackInterval = 3.0f;
	GEProjectile* _projectiles[MAX_PROJECTILES];
	int _projectileCount = 0;

	bool collidesWithWater(int newX, int newY, const GEMapsManager& mapsManager) const;

	bool collidesWithEnemies(int newX, int newY, const GEEnemyManager& enemyManager) const;

public:

	GEPlayer();
	~GEPlayer();

	void loadData(GESaveData* saveData);
	
	void update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight, const GEMapsManager& mapsManager, const GEEnemyManager& enemyManager);

	void updateAttack(float deltaTime, const GEEnemyManager& enemyManager, GEProjectileManager& projectileManager);

};
