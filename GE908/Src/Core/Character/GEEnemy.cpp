#include "GEEnemy.h"
#include "../../Foundation/GELog.h"
#include "../../Foundation/GEWindow.h"

GEEnemy::GEEnemy(GEEnemyType type) {
    _type = type;

    switch (_type) {
    case Normal:
        load("Src/Assets/Textures/enemy_normal.png");
        _hp = 100;
        _speed = 80;
        break;
    case Fast:
        load("Src/Assets/Textures/enemy_fast.png");
        _hp = 60;
        _speed = 150;
        break;
    case Heavy:
        load("Src/Assets/Textures/enemy_heavy.png");
        _hp = 200;
        _speed = 50;
        break;
    case StaticShooter:
        load("Src/Assets/Textures/enemy_static.png");
        _hp = 80;
        _speed = 0;
        _isStatic = true;
        _attackRate = 3.0f;
        break;
    }
}

GEEnemy::~GEEnemy() {}

void GEEnemy::update(float deltaTime, int playerX, int playerY) {
    if (!isAlive) return;

    if (!_isStatic) {
        int dirX = (playerX > _originX) - (playerX < _originX);
        int dirY = (playerY > _originY) - (playerY < _originY);

        float moveDelta = _speed * deltaTime;
        _originX += static_cast<int>(dirX * moveDelta);
        _originY += static_cast<int>(dirY * moveDelta);
    }
    else {
        _attackCooldown += deltaTime;
        if (_attackCooldown > _attackRate) {
            _attackCooldown = 0;
            GELog::shared().info("Static enemy fired projectile");
        }
    }
}

void GEEnemy::draw(GEWindow& window) {
    if (_characterImage) {}
        // window.draw(_x, _y , *_characterImage);
}
