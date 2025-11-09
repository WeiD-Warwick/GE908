#include "GEPowerUpManager.h"
#include "../Entity/GEPlayer.h"
#include <cstdlib>
#include <ctime>

constexpr const char* ATTACK_SPEED_TEXTURE = "Src/Assets/Textures/attack_speed.png";
constexpr const char* AOE_TARGET_TEXTURE = "Src/Assets/Textures/aoe_target.png";

// ------------------ GEPowerUp ------------------
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
    default:
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
        _remainingTime -= deltaTime;
        if (_remainingTime <= 0.0f)
            deactivate();
    }
}

// ------------------ GEPowerUpManager ------------------
GEPowerUpManager::GEPowerUpManager() {
    _powerUps.resize(16);     // max 16 power-ups
    _powerUps.fillNull(16);   // initialize with nullptr
}

GEPowerUpManager::~GEPowerUpManager() {
    _powerUps.destroyAll();
    _powerUps.clear();
}

void GEPowerUpManager::load(GESaveData* saveData) {
    _saveData = saveData;
    _spawnTimer = 0.0f;
    _powerUps.destroyAll();
    _powerUps.fillNull(16);
}

void GEPowerUpManager::spawnPowerUpAt(const GEPoint& point) {
    // Random drop chance
    if (randomFloat(0.0f, 1.0f) > DROP_CHANCE) return;

    float offsetX = randomFloat(-20.0f, 20.0f);
    float offsetY = randomFloat(-20.0f, 20.0f);
    float spawnX = point.x + offsetX;
    float spawnY = point.y + offsetY;

    GEPowerUpType type = (randomFloat(0.0f, 1.0f) < 0.5f)
        ? GEPowerUpType::AttackSpeedBoost
        : GEPowerUpType::AdditionalAoeTarget;

    // Try to reuse inactive slot
    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        if (_powerUps[i] == nullptr) {
            _powerUps[i] = new GEPowerUp();
        }
        if (!_powerUps[i]->isAlive()) {
            _powerUps[i]->spawn(type, spawnX, spawnY, POWERUP_LIFETIME_SECONDS);
            return;
        }
    }
}

void GEPowerUpManager::update(float deltaTime, GEContext& ctx) {
    if (!_saveData) return;

    _spawnTimer += deltaTime;
    if (_spawnTimer >= SPAWN_INTERVAL_SECONDS) {
        _spawnTimer = 0.0f;
        // could spawn periodic global pickups here if desired
    }

    GEPlayer& player = static_cast<GEPlayer&>(ctx.playerProvider());

    _powerUps.forEachActive([&](GEPowerUp* p, unsigned int) {
        if (!p) return;
        p->update(deltaTime);

        if (p->isAlive() && p->collide(player.collisionBody())) {
            player.applyPowerUp(p->getType());
            p->deactivate();
        }
        });

    _powerUps.destroyInactive();
}

void GEPowerUpManager::draw(Window& window, const GECamera& camera) {
    _powerUps.forEachActive([&](GEPowerUp* p, unsigned int) {
        if (p && p->isAlive())
            p->draw(window, camera);
        });
}

void GEPowerUpManager::onEnemyDefeated(const GEPoint& position) {
    spawnPowerUpAt(position);
}
