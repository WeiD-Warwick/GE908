#pragma once
#include "BaseCharacter.h"
#include "../Camera/GECamera.h"
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GESaveData.h"

enum GEEnemyType {
    Normal,
    Fast,
    Heavy,
    StaticShooter,
};

class GEEnemy : public BaseCharacter {
private:
    GEEnemyType _type;
    float _attackCooldown = 0.0f;
    float _attackRate = 2.0f;
    bool _isStatic = false;
    int _vx = 0;
    int _vy = 0;

public:
    GEEnemy() {}
    GEEnemy(GEEnemyType type);
    ~GEEnemy();

    void update(float deltaTime, int playerX, int playerY);
    void draw(GEWindow& window);

    GEEnemyType getType() const { return _type; }
    bool getIsStatic() const { return _isStatic; }
};
