#pragma once
#include "GECharacter.h"
#include "../Interface/GEProvider.h"
#include "../../Foundation/GECamera.h"
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GESaveData.h"

static constexpr auto MAX_PROJECTILES = 100;
static constexpr auto PLAYER_PROJECTILE_SPEED = 150.0f;
static constexpr auto PLAYER_PROJECTILE_DAMAGE = 20;
static constexpr auto PLAYER_MAX_AOE_EFFECTS = 10;
static constexpr auto PLAYER_MAX_AOE_TARGETS = 8;
static constexpr auto PLAYER_MAX_AUTO_ATTACK_SPEED_MULTIPLIER = 3.0f;

class GEEnemy;

class GEPlayer : public GECharacter, public PlayerProvider {

private:
	GESaveData* _saveData = nullptr;
	MapProvider* _mapsManager = nullptr;
	EnemyProvider* _enemyManager = nullptr;
	ProjectileProvider* _projectileManager = nullptr;
	PowerUpProvider* _powerUpManager = nullptr;

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

	struct PlayerAoeEffect {
		bool active = false;
		float centerX = 0.0f;
		float centerY = 0.0f;
		float radius = 0.0f;
		GEColor color = GEColor();
		float remainingTime = 1.0f;
	};

	PlayerAoeEffect _aoeEffects[PLAYER_MAX_AOE_EFFECTS];

	bool collidesWithTileType(float newX, float newY, const MapProvider& Manager, GECollisionType targetType) const;

	bool collidesWithEnemies(float newX, float newY, const EnemyProvider& enemyManager) const;

	bool isBlockedAt(float x, float y) const;

	void applyEnvironmentalEffects(float deltaTime);

	void applyMovementBounds(float& newX, float& newY) override;

	void autoAttack(float deltaTime);

	void aoeAttack(float deltaTime);
	void executeAoeSkill();
	int findEnemiesWithinRadius(float cx, float cy, float radius, GEEnemy** outList, int maxCount) const;
	int selectTopEnemiesByHP(GEEnemy** input, int count, int topN, GEEnemy** output) const;
	void updateAoeEffects(float deltaTime);
	void spawnAoeEffect(float centerX, float centerY, float radius, GEColor color);
	void drawAoeIndicatorIfNeeded(Window& window, const GECamera& camera) const;
	void drawAoeEffects(Window& window, const GECamera& camera) const;
	void drawCircle(Window& window, const GECamera& camera, float centerX, float centerY, float radius, GEColor color) const;

public:

	GEPlayer();

	~GEPlayer() = default;

	void bind(GEContext& ctx) override;

	void update(float deltaTime, Window& window);

	void draw(Window& window, const GECamera& camera) const override;

	float getAOECooldownTime() const { return _aoeCooldownTimer;}

	void applyPowerUp(GEPowerUpType type);

	void takeDamage(int value) override;

	GECollisible& collisionBody() { return *this; }

	int getHP() const override { return _hp; }

};
