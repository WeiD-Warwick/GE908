#pragma once
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEObjectPool.h"

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
