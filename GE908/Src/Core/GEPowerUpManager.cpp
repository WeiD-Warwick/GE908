#include <iostream>
#include "GEPowerUpManager.h"
#include "GEPlayer.h"
#include "GEEnemyManager.h"
#include "GEEnemy.h"

namespace {
    constexpr float PI = 3.14159265358979323846f;
    constexpr const char* ATTACK_SPEED_TEXTURE = "Src/Assets/Textures/attack_speed.png";
    constexpr const char* AOE_TARGET_TEXTURE = "Src/Assets/Textures/aoe_target.png";
}

GEPowerUp::GEPowerUp()
    : GECollisible(ATTACK_SPEED_TEXTURE, GECollisionType::PowerUp) {
}

void GEPowerUp::spawn(GEPowerUpType type, float centerX, float centerY, float lifeTimeSeconds) {
    _type = type;
    _timeToLive = lifeTimeSeconds;
    _remainingTime = lifeTimeSeconds;
    _active = true;
    switch (_type) {
    case GEPowerUpType::AttackSpeedBoost:
        _image.load(ATTACK_SPEED_TEXTURE);
        break;
    case GEPowerUpType::AdditionalAoeTarget:
        _image.load(AOE_TARGET_TEXTURE);
        break;
    }
    setCenter(centerX, centerY);
}

void GEPowerUp::deactivate() {
    _active = false;
    _remainingTime = 0.0f;
}

void GEPowerUp::update(float deltaTime) {
    if (!_active) return;

    if (_timeToLive > 0.0f) {
        _remainingTime = max(0.0f, _remainingTime - deltaTime);
        if (_remainingTime <= 0.0f) {
            deactivate();
        }
    }
}

GEPowerUpManager::GEPowerUpManager() {
    for (int i = 0; i < MAX_POWERUPS; ++i) {
        _powerUps[i] = nullptr;
    }
}

GEPowerUpManager::~GEPowerUpManager() {
    for (int i = 0; i < MAX_POWERUPS; ++i) {
        delete _powerUps[i];
        _powerUps[i] = nullptr;
    }
}

void GEPowerUpManager::load(GESaveData* saveData) {
    _saveData = saveData;
    _spawnTimer = 0.0f;
}

bool GEPowerUpManager::isWaterTile(int tileID) const {
    return tileID >= 14 && tileID <= 22;
}

void GEPowerUpManager::spawnPowerUpFromEnemy(const GEEnemy& enemy) {

    // drop rate
    const float dropChance = 0.3f;
    if (randomFloat(0.0f, 1.0f) > dropChance) return;

    float ex = enemy.getCenterX();
    float ey = enemy.getCenterY();

    float offsetX = randomFloat(-20.0f, 20.0f);
    float offsetY = randomFloat(-20.0f, 20.0f);

    float spawnX = ex + offsetX;
    float spawnY = ey + offsetY;

    GEPowerUpType type = (randomFloat(0.0f, 1.0f) < 0.5f)
        ? GEPowerUpType::AttackSpeedBoost
        : GEPowerUpType::AdditionalAoeTarget;

    for (int i = 0; i < MAX_POWERUPS; ++i) {
        if (_powerUps[i] == nullptr) {
            _powerUps[i] = new GEPowerUp();
        }
        if (!_powerUps[i]->isActive()) {
            _powerUps[i]->spawn(type, spawnX, spawnY, 10.0f);
            return;
        }
    }
}

void GEPowerUpManager::update(float deltaTime, GEPlayer& player, const GEEnemyManager& enemyManager) {
    if (!_saveData) return;

    _spawnTimer += deltaTime;
    if (_spawnTimer >= SPAWN_INTERVAL_SECONDS) {
        _spawnTimer = 0.0f;
    }

    for (int i = 0; i < MAX_POWERUPS; ++i) {
        GEPowerUp* powerUp = _powerUps[i];
        if (!powerUp || !powerUp->isActive()) continue;

        powerUp->update(deltaTime);
        if (!powerUp->isActive()) continue;

        if (powerUp->collide(player)) {
            player.applyPowerUp(powerUp->getType());
            powerUp->deactivate();
        }
    }
}

void GEPowerUpManager::draw(Window& window, const GECamera& camera) {
    for (int i = 0; i < MAX_POWERUPS; ++i) {
        GEPowerUp* powerUp = _powerUps[i];
        if (!powerUp || !powerUp->isActive()) continue;
        powerUp->draw(window, camera);
    }
}