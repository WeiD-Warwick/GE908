#include "GEPowerUp.h"

GEPowerUp::GEPowerUp()
    : GECollisible(PowerUp::ATTACK_SPEED_TEXTURE, GECollisionType::PowerUp) {
}

void GEPowerUp::spawn(GEPowerUpType type, float centerX, float centerY, float lifeTimeSeconds) {
    _type = type;
    _timeToLive = lifeTimeSeconds;
    _remainingTime = lifeTimeSeconds;
    _active = true;

    switch (_type) {
    case GEPowerUpType::AttackSpeedBoost:
        _image.load(PowerUp::ATTACK_SPEED_TEXTURE);
        break;
    case GEPowerUpType::AdditionalAoeTarget:
        _image.load(PowerUp::AOE_TARGET_TEXTURE);
        break;
    case GEPowerUpType::HealPlayer:
        _image.load(PowerUp::HEAL_TEXTURE);
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

GEPowerUpState GEPowerUp::snapshotState() const {
    GEPowerUpState state;
    state.type = _type;
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.timeToLive = _timeToLive;
    state.remainingTime = _remainingTime;
    if (_active) state.activate();
    else state.deactivate();
    return state;
}

void GEPowerUp::applyState(const GEPowerUpState& state) {
    _type = state.type;
    _timeToLive = state.timeToLive;
    _remainingTime = state.remainingTime;
    _active = true;

    switch (_type) {
    case GEPowerUpType::AttackSpeedBoost:
        _image.load(PowerUp::ATTACK_SPEED_TEXTURE);
        break;
    case GEPowerUpType::AdditionalAoeTarget:
        _image.load(PowerUp::AOE_TARGET_TEXTURE);
        break;
    case GEPowerUpType::HealPlayer:
        _image.load(PowerUp::HEAL_TEXTURE);
        break;
    default:
        break;
    }

    setCenter(state.centerX, state.centerY);
}