#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Foundation/GECollisible.h"
#include "../Foundation/GESaveData.h"
#include "../Foundation/GECamera.h"
#include "GEPlayer.h"
#include "GEEnemy.h"

using namespace GamesEngineeringBase;

class GEPowerUp : public GECollisible {
private:
    bool _active = false;
    float _timeToLive = 0.0f;
    float _remainingTime = 0.0f;
    GEPowerUpType _type = GEPowerUpType::None;

public:
    GEPowerUp();

    void spawn(GEPowerUpType type, float centerX, float centerY, float lifeTimeSeconds);
    void deactivate();
    bool isActive() const { return _active; }
    GEPowerUpType getType() const { return _type; }

    void update(float deltaTime);
};

class GEPowerUpManager : public PowerUpService {
private:
    static constexpr int MAX_POWERUPS = 16;
    static constexpr float SPAWN_INTERVAL_SECONDS = 10.0f;
    static constexpr float POWERUP_LIFETIME_SECONDS = 18.0f;
    static constexpr float MIN_SPAWN_DISTANCE = 60.0f;
    static constexpr float MAX_SPAWN_DISTANCE = 320.0f;

    GEPowerUp* _powerUps[MAX_POWERUPS];
    GESaveData* _saveData = nullptr;
    float _spawnTimer = 0.0f;

    bool isWaterTile(int tileID) const;
    void spawnPowerUpAt(const GEPoint point);

public:
    GEPowerUpManager();
    ~GEPowerUpManager();

    void load(GESaveData* saveData);
    void update(float deltaTime, PlayerService& player);
    void draw(Window& window, const GECamera& camera);
};
