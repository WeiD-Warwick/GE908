#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Foundation/GECollisible.h"
#include "../Foundation/GESaveData.h"
#include "../Foundation/GECamera.h"
#include "../Foundation/GEObjectPool.h"
#include "GEPlayer.h"
#include "GEEnemy.h"

using namespace GamesEngineeringBase;

class GEPowerUp : public GECollisible, public GEPoolable {
private:
    bool _active = false;
    float _timeToLive = 0.0f;
    float _remainingTime = 0.0f;
    GEPowerUpType _type = GEPowerUpType::None;

public:
    GEPowerUp();
    void spawn(GEPowerUpType type, float centerX, float centerY, float lifeTimeSeconds);
    void deactivate();
    bool isAlive() const { return _active; }
    bool isActiveElement() const override { return _active; }
    GEPowerUpType getType() const { return _type; }
    void update(float deltaTime);
};

class GEPowerUpManager : public PowerUpProvider {
private:
    static constexpr float SPAWN_INTERVAL_SECONDS = 10.0f;
    static constexpr float POWERUP_LIFETIME_SECONDS = 18.0f;
    static constexpr float DROP_CHANCE = 0.3f;

    GEObjectPool<GEPowerUp*> _powerUps;
    GESaveData* _saveData = nullptr;
    float _spawnTimer = 0.0f;

    void spawnPowerUpAt(const GEPoint& point);

public:
    GEPowerUpManager();
    ~GEPowerUpManager();

    void load(GESaveData* saveData);
    void update(float deltaTime, GEContext& ctx);
    void draw(Window& window, const GECamera& camera);
    void onEnemyDefeated(const GEPoint& position) override;
};
