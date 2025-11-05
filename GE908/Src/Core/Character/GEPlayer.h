#pragma once
#include "BaseCharacter.h"
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GESaveData.h"
#include "../Items/GEProjectile.h"
#include "../Items/GEBuffManager.h"

#define MAX_PROJECTILES 20

class GEMapsManager;

class GEEnemyManager;

class GEProjectileManager;

class GEPlayer : public BaseCharacter {
private:
	GESaveData* _saveData = nullptr;

	float _attackTimer = 0.0f;
	float _attackIntervalBase = 3.0f;
	float _attackSpeedMultiplier = 1.0f;
	float _attackSpeedBuffTimer = 0.0f;

	float _skillTimer = 0.0f;
	float _skillCooldown = 12.0f;
	int _baseSkillTargetCount = 3;
	int _bonusSkillTargets = 0;
	float _skillBuffTimer = 0.0f;
	float _skillRadius = 120.0f;
	int _skillDamage = 150;

	GEProjectile* _projectiles[MAX_PROJECTILES];
	int _projectileCount = 0;

	bool collidesWithWater(int newX, int newY, const GEMapsManager& mapsManager) const;

	bool collidesWithEnemies(int newX, int newY, const GEEnemyManager& enemyManager) const;

	void updateBuffTimers(float deltaTime);

	float getEffectiveAttackInterval() const;

public:

	GEPlayer();
	~GEPlayer();

	void loadData(GESaveData* saveData);
	
	void update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight, const GEMapsManager& mapsManager, const GEEnemyManager& enemyManager);

	void updateAttack(float deltaTime, const GEEnemyManager& enemyManager, GEProjectileManager& projectileManager);
	
	void updateSkill(float deltaTime, bool triggerSkill, GEEnemyManager& enemyManager);

	void applyBuff(GEBuffType type);

};
