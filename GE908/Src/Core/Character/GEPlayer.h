#pragma once
#include "BaseCharacter.h"
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GESaveData.h"
#include "../Items/GEProjectile.h"

#define MAX_PROJECTILES 20
#define SHOW_PLAYER_AOE_INDICATOR false
#define PLAYER_MAX_AOE_EFFECTS 8
#define PLAYER_MAX_AOE_TARGETS 16

class GEMapsManager;

class GEEnemyManager;

class GEProjectileManager;

class GECamera;

class GEPlayer : public BaseCharacter {
private:
	const GESaveData* _saveData = nullptr;
	const GEMapsManager* _mapsManager = nullptr;
	const GEEnemyManager* _enemyManager = nullptr;
	GEProjectileManager* _projectileManager = nullptr;

	// auto attack
	float _autoAttackTimer = 0.0f;
	float _autoAttackIntervalBase = 2.0f;
	float _autoAttackSpeedMultiplier = 1.0f;

	// aoe
	float _aoeCooldownTimer = 0.0f;
	float _aoeCooldown = 6.0f;
	float _aoeRadius = 160.0f;
	float _aoeEffectDuration = 0.4f;
	int _aoeDamage = 200;
	int _aoeTargetCount = 3;
	bool _aoeKeyHeld = false;


	GEProjectile* _projectiles[MAX_PROJECTILES];
	int _projectileCount = 0;

	struct PlayerAoeEffect {
		bool active = false;
		float centerX = 0.0f;
		float centerY = 0.0f;
		float radius = 0.0f;
		unsigned char colorR = 255;
		unsigned char colorG = 255;
		unsigned char colorB = 255;
		float remainingTime = 0.0f;
	};

	PlayerAoeEffect _aoeEffects[PLAYER_MAX_AOE_EFFECTS];

	bool collidesWithWater(int newX, int newY, const GEMapsManager& mapsManager) const;

	bool collidesWithEnemies(int newX, int newY, const GEEnemyManager& enemyManager) const;

	bool isBlockedAt(float x, float y) const;

	void applyMovementBounds(float& newX, float& newY) override;

	void autoAttack(float deltaTime);

	void aoeAttack(float deltaTime);
	void castAoeSkill();
	void updateAoeEffects(float deltaTime);
	void spawnAoeEffect(float centerX, float centerY, float radius, unsigned char r, unsigned char g, unsigned char b);
	void drawAoeIndicator(GEWindow& window, const GECamera& camera) const;
	void drawAoeEffects(GEWindow& window, const GECamera& camera) const;
	void drawCircle(GEWindow& window, const GECamera& camera, float centerX, float centerY, float radius, unsigned char r, unsigned char g, unsigned char b) const;
	

public:

	GEPlayer();
	~GEPlayer();

	void bindWorldContext(const GEMapsManager* maps, const GEEnemyManager* enemies, GEProjectileManager* projectiles);
	
	void update(float deltaTime, GEWindow& window);

	void draw(GEWindow& window, const GECamera& camera);

};
