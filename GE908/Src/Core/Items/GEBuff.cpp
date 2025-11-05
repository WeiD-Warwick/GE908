#include "GEBuff.h"

static constexpr float DEFAULT_BUFF_LIFETIME = 12.0f;

const char* GEBuff::textureForType(GEBuffType type) {
    switch (type) {
    case GEBuffType::AttackSpeed:
        return "Src/Assets/Textures/enemy_bullet.png";
    case GEBuffType::SkillTargets:
        return "Src/Assets/Textures/arrow.png";
    default:
        return "Src/Assets/Textures/enemy_bullet.png";
    }
}

GEBuff::GEBuff(GEBuffType type)
    : GECollisible(textureForType(type), None), _type(type) {
}

void GEBuff::activateAt(int worldX, int worldY, float lifeTimeSeconds) {
    int adjustedX = worldX - _image.width / 2;
    int adjustedY = worldY - _image.height / 2;
    setCenter(adjustedX, adjustedY);
    _lifeTime = lifeTimeSeconds > 0.0f ? lifeTimeSeconds : DEFAULT_BUFF_LIFETIME;
    _elapsed = 0.0f;
    _active = true;
}

void GEBuff::deactivate() {
    _active = false;
    _elapsed = 0.0f;
    _lifeTime = 0.0f;
}

void GEBuff::tick(float deltaTime) {
    if (!_active) return;
    _elapsed += deltaTime;
    if (_elapsed >= _lifeTime) {
        deactivate();
    }
}
