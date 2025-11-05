#pragma once

#include "../../Foundation/GECollisible.h"

enum class GEBuffType {
    AttackSpeed,
    SkillTargets
};

class GEBuff : public GECollisible {
private:
    GEBuffType _type;
    bool _active = false;
    float _lifeTime = 0.0f;
    float _elapsed = 0.0f;

    static const char* textureForType(GEBuffType type);

public:
    GEBuff(GEBuffType type);
    ~GEBuff() override = default;

    void activateAt(int worldX, int worldY, float lifeTimeSeconds);
    void deactivate();
    void tick(float deltaTime);

    bool isActive() const { return _active; }
    GEBuffType getType() const { return _type; }
};