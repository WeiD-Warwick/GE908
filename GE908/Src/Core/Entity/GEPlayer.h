#pragma once
#include "GECharacter.h"
#include "GETile.h"
#include "../../Foundation/GEConst.h"
#include "../Interface/GEProvider.h"
#include "../../Foundation/GECamera.h"
#include "../../Foundation/GEModel.h"
#include "../SaveLoad/GESaveData.h"

class GEEnemy;

class GEPlayer : public GECharacter, public PlayerProvider, public GECodable<GEPlayerState> {

private:
	GESaveData* _saveData = nullptr;
	MapProvider* _mapsManager = nullptr;
	EnemyProvider* _enemyManager = nullptr;
	ProjectileProvider* _projectileManager = nullptr;
	PowerUpProvider* _powerUpManager = nullptr;

	// auto attack
	float _autoAttackTimer = 0.0f;
	float _autoAttackIntervalBase = Player::PLAYER_BASE_AUTO_ATTACK_INTERVAL;
	float _autoAttackSpeedMultiplier = 1.0f;
	float _attackSpeedBuffTimer = 0.0f;

	// aoe
	float _aoeCooldownTimer = 0.0f;
	float _aoeCooldown = 5.0f;
	float _aoeRadius = 300.0f;
	float _aoeEffectDuration = 0.35f;
	int _aoeTargetCount = Player::PLAYER_BASE_AOE_TARGETS;
	float _aoeTargetBuffTimer = 0.0f;
	bool _aoeKeyHeld = false;

	struct AoeImpact {
		float x = 0.0f;
		float y = 0.0f;
		float remainingTime = 0.0f;
	};

	AoeImpact _aoeImpacts[Player::PLAYER_MAX_AOE_EFFECTS];
	int _aoeImpactCount = 0;

	bool collidesWithTileType(float newX, float newY, const MapProvider& Manager, GECollisionType targetType) const;

	bool collidesWithEnemies(float newX, float newY, const EnemyProvider& enemyManager) const;

	bool isBlockedAt(float x, float y) const;

	void applyEnvironmentalEffects(float deltaTime);

	void applyMovementBounds(float& newX, float& newY) override;

	void autoAttack(float deltaTime);

	void aoeAttack(float deltaTime);

	void executeAoeSkill();
	void updateAoeImpacts(float deltaTime);
	void recordAoeImpact(float centerX, float centerY);
	void drawAoeImpacts(Window& window, const GECamera& camera) const;
	void drawImpact(Window& window, const GECamera& camera, float centerX, float centerY, float radius, GEColor color) const;

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

	GEPlayerState snapshotState() const override;
	void applyState(const GEPlayerState& state) override;

};
