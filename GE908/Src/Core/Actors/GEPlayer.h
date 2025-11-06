#pragma once
#include "BaseCharacter.h"
#include "../../Foundation/GESaveData.h"

static constexpr auto SHOW_PLAYER_AOE_INDICATOR = true;

static constexpr auto MAX_PROJECTILES = 100;
static constexpr auto PLAYER_PROJECTILE_SPEED = 150.0f;
static constexpr auto PLAYER_PROJECTILE_DAMAGE = 20;
static constexpr auto PLAYER_MAX_AOE_EFFECTS = 10;
static constexpr auto PLAYER_MAX_AOE_TARGETS = 3;

class GEMapsManager;

class GEEnemy;

class GEEnemyManager;

class GEProjectileManager;

class GEProjectile;

class GECamera;

class GEPlayer : public BaseCharacter {
private:
	const GESaveData* _saveData = nullptr;
	const GEMapsManager* _mapsManager = nullptr;
	const GEEnemyManager* _enemyManager = nullptr;
	GEProjectileManager* _projectileManager = nullptr;

	// auto attack
	float _autoAttackTimer = 0.0f;
	float _autoAttackIntervalBase = 1.4f;
	float _autoAttackSpeedMultiplier = 1.0f;

	// aoe
	float _aoeCooldownTimer = 0.0f;
	float _aoeCooldown = 5.0f;
	float _aoeRadius = 300.0f;
	float _aoeEffectDuration = 0.35f;
	int _aoeDamage = 220;
	int _aoeTargetCount = 4;
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
		float remainingTime = 1.0f;
	};

	PlayerAoeEffect _aoeEffects[PLAYER_MAX_AOE_EFFECTS];

	bool collidesWithWater(float newX, float newY, const GEMapsManager& mapsManager) const;

	bool collidesWithEnemies(float newX, float newY, const GEEnemyManager& enemyManager) const;

	bool isBlockedAt(float x, float y) const;

	void applyMovementBounds(float& newX, float& newY) override;

	void autoAttack(float deltaTime);

	void aoeAttack(float deltaTime);
	void executeAoeSkill();
	int findEnemiesWithinRadius(float cx, float cy, float radius, GEEnemy** outList, int maxCount) const;
	int selectTopEnemiesByHP(GEEnemy** input, int count, int topN, GEEnemy** output) const;
	void updateAoeEffects(float deltaTime);
	void spawnAoeEffect(float centerX, float centerY, float radius, unsigned char r, unsigned char g, unsigned char b);
	void drawAoeIndicator(Window& window, const GECamera& camera) const;
	void drawAoeEffects(Window& window, const GECamera& camera) const;
	void drawCircle(Window& window, const GECamera& camera, float centerX, float centerY, float radius, unsigned char r, unsigned char g, unsigned char b) const;
	
public:

	GEPlayer();
	~GEPlayer();

	void bindWorldContext(const GEMapsManager* maps, const GEEnemyManager* enemies, GEProjectileManager* projectiles);
	
	void update(float deltaTime, Window& window);

	void draw(Window& window, const GECamera& camera);

	float getAOECooldownTime() const { return _aoeCooldownTimer;}

};
