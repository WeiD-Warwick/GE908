#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "GEPowerUp.h"
#include "GEPlayer.h"
#include "GEEnemyManager.h"
#include "GEEnemy.h"

namespace {
    constexpr float PI = 3.14159265358979323846f;
    constexpr const char* ATTACK_SPEED_TEXTURE = "Src/Assets/Textures/attack_speed.png";
    constexpr const char* AOE_TARGET_TEXTURE = "Src/Assets/Textures/aoe_target.png";

    float randomFloat() {
        return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    }
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

bool GEPowerUpManager::findSpawnLocation(float& outX, float& outY, const GEPlayer& player, const GEEnemyManager& enemyManager) const {
    if (!_saveData) return false;

    const int mapWidth = _saveData->getMapTotalWidth();
    const int mapHeight = _saveData->getMapTotalHeight();
    const int tileWidth = _saveData->getTileWidth();
    const int tileHeight = _saveData->getTileHeight();

    const int attempts = 24;
    const float playerX = player.getCenterX();
    const float playerY = player.getCenterY();

    for (int attempt = 0; attempt < attempts; ++attempt) {
        const float angle = randomFloat() * 2.0f * PI;
        const float distance = MIN_SPAWN_DISTANCE + randomFloat() * (MAX_SPAWN_DISTANCE - MIN_SPAWN_DISTANCE);
        float candidateX = playerX + std::cos(angle) * distance;
        float candidateY = playerY + std::sin(angle) * distance;

        candidateX = max(0.0f, min(candidateX, static_cast<float>(mapWidth - 1)));
        candidateY = max(0.0f, min(candidateY, static_cast<float>(mapHeight - 1)));

        const int col = static_cast<int>(candidateX) / tileWidth;
        const int row = static_cast<int>(candidateY) / tileHeight;
        if (col < 0 || row < 0 || col >= _saveData->getMapColCount() || row >= _saveData->getMapRowCount()) {
            continue;
        }

        const int tileID = _saveData->getTileID(0, row, col);
        if (isWaterTile(tileID)) {
            continue;
        }

        bool blocked = false;
        const int enemyCount = enemyManager.getEnemyCount();
        const float minEnemyDistance = 48.0f;
        const float minEnemyDistSq = minEnemyDistance * minEnemyDistance;
        for (int i = 0; i < enemyCount; ++i) {
            GEEnemy* enemy = enemyManager.getEnemyAt(i);
            if (!enemy || !enemy->isAlive()) continue;
            const float dx = enemy->getCenterX() - candidateX;
            const float dy = enemy->getCenterY() - candidateY;
            if (dx * dx + dy * dy < minEnemyDistSq) {
                blocked = true;
                break;
            }
        }
        if (blocked) {
            continue;
        }

        const float minPlayerDistance = 64.0f;
        const float dxPlayer = playerX - candidateX;
        const float dyPlayer = playerY - candidateY;
        if (dxPlayer * dxPlayer + dyPlayer * dyPlayer < minPlayerDistance * minPlayerDistance) {
            continue;
        }

        outX = candidateX;
        outY = candidateY;
        return true;
    }

    return false;
}

void GEPowerUpManager::spawnPowerUp(const GEPlayer& player, const GEEnemyManager& enemyManager) {
    float spawnX = 0.0f;
    float spawnY = 0.0f;
    if (!findSpawnLocation(spawnX, spawnY, player, enemyManager)) {
        return;
    }

    int slot = -1;
    for (int i = 0; i < MAX_POWERUPS; ++i) {
        if (_powerUps[i] == nullptr) {
            _powerUps[i] = new GEPowerUp();
            slot = i;
            break;
        }
        if (!_powerUps[i]->isActive()) {
            slot = i;
            break;
        }
    }

    if (slot < 0) {
        return;
    }

    if (_powerUps[slot] == nullptr) {
        _powerUps[slot] = new GEPowerUp();
    }

    const GEPowerUpType type = (randomFloat() < 0.5f)
        ? GEPowerUpType::AttackSpeedBoost
        : GEPowerUpType::AdditionalAoeTarget;
    _powerUps[slot]->spawn(type, spawnX, spawnY, POWERUP_LIFETIME_SECONDS);
}

void GEPowerUpManager::update(float deltaTime, GEPlayer& player, const GEEnemyManager& enemyManager) {
    if (!_saveData) return;

    _spawnTimer += deltaTime;
    if (_spawnTimer >= SPAWN_INTERVAL_SECONDS) {
        spawnPowerUp(player, enemyManager);
        _spawnTimer = 0.0f;
    }

    for (int i = 0; i < MAX_POWERUPS; ++i) {
        GEPowerUp* powerUp = _powerUps[i];
        if (!powerUp || !powerUp->isActive()) continue;

        powerUp->update(deltaTime);
        if (!powerUp->isActive()) continue;

        if (powerUp->collide(player)) {
            player.applyPowerUp(powerUp->getType());
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