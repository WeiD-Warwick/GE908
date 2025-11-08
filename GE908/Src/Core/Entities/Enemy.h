#pragma once
#include "Character.h"
#include "../../Foundation/GEEvent.h"
#include "../../Foundation/GEContext.h"
#include "../../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class GEEnemy : public Character {
private:
    GEEnemyType _type;
    bool _isStatic = false;
    float _attackCooldown = 0.0f;
    float _attackRate = 2.0f;

public:
    GEEnemy(GEEnemyType type, GEPoint spawn, GEContext* ctx)
        : Character(spawn, GEColliderType::Enemy, GEColliderShape::Circle, 150.f, ctx),
        _type(type)
    {
        static Image enemyImg;
        enemyImg.load(getEnemyImagePath(type));
        _render.setImage(&enemyImg);
        _render.setType(GEColliderType::Enemy);
        _render.showHP(true);

        switch (_type) {
        case GEEnemyType::Normal:
            _health.reset(200, 200);
            _move.setSpeed(150.f);
            break;
        case GEEnemyType::Fast:
            _health.reset(100, 100);
            _move.setSpeed(270.f);
            break;
        case GEEnemyType::Heavy:
            _health.reset(300, 300);
            _move.setSpeed(80.f);
            break;
        case GEEnemyType::StaticShooter:
            _health.reset(150, 150);
            _move.setSpeed(0.f);
            _isStatic = true;
            _attackRate = 2.8f;
            break;
        }
    }

    ~GEEnemy() override = default;

    void update(float dt, const GEPoint& playerCenter) {
        if (!_health.isAlive()) return;

        if (!_isStatic)
            moveTowardPlayer(dt, playerCenter);
        else
            tryShootAtPlayer(dt, playerCenter); // ? ? dt

        Character::update(dt);
    }


    void render(Window& window, const GECamera& camera) const override {
        Character::render(window, camera);
    }

    void takeDamage(int dmg) override {
        if (dmg <= 0) return;
        Character::takeDamage(dmg);
        _render.triggerFlash(GEColor(255, 255, 255), 0.25f);
    }

    void onDeath() override {
        printf("[Enemy] Died (type=%d)\n", static_cast<int>(_type));
    }

private:
    static std::string getEnemyImagePath(GEEnemyType t) {
        switch (t) {
        case GEEnemyType::Normal:        return "Src/Assets/Textures/enemy_normal.png";
        case GEEnemyType::Fast:          return "Src/Assets/Textures/enemy_fast.png";
        case GEEnemyType::Heavy:         return "Src/Assets/Textures/enemy_heavy.png";
        case GEEnemyType::StaticShooter: return "Src/Assets/Textures/enemy_static.png";
        default:                         return "Src/Assets/Textures/enemy_normal.png";
        }
    }

    void moveTowardPlayer(float dt, const GEPoint& playerCenter) {
        GEPoint pos = _move.getOriginPosition();
        float dx = playerCenter.x - pos.x;
        float dy = playerCenter.y - pos.y;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len <= 0.001f) return;
        dx /= len;
        dy /= len;
        _move.move(dt, GEDirection{ dx, dy });
    }

    void tryShootAtPlayer(float dt, const GEPoint& playerCenter) {
        _attackCooldown += dt;
        if (_attackCooldown < _attackRate) return;
        _attackCooldown = 0.0f;

        GEPoint pos = _move.getOriginPosition();
        float dx = playerCenter.x - pos.x;
        float dy = playerCenter.y - pos.y;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len <= 0.001f) return;

        dx /= len;
        dy /= len;

        //if (_ctx && _ctx->notification()) {
        //    GEEnemyAttackEvent ev(this, pos, GEPoint(dx, dy));
        //    _ctx->notification()->post(ev);
        //}
    }

};
