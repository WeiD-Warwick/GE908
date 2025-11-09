#include "GEPowerUp.h"

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