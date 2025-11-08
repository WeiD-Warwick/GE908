#pragma once
#include "GEModel.h"

struct GEEvent {
    GEEventType type;
    GEEvent(GEEventType t) : type(t) {}
};

struct GECharacterDeathEvent : GEEvent {
    GECharacterDeathEvent(int id) : GEEvent(GEEventType::Character_DEATH) {}
};

struct GEPlayerHurtEvent : GEEvent {
    int newHP;
    int damage;
    GEPlayerHurtEvent(int hp, int dmg) : GEEvent(GEEventType::PLAYER_HURT), newHP(hp), damage(dmg) {}
};

struct GEEnemyAttackEvent : GEEvent {
    int enemyId;
    GEPoint position;
    GEPoint direction;
    GEEnemyAttackEvent(int id, GEPoint pos, GEPoint dir)
        : GEEvent(GEEventType::ENEMY_ATTACK), enemyId(id), position(pos), direction(dir) {
    }
};


struct GEPowerUpPickedEvent : GEEvent {
    int powerUpId;
    GEPowerUpPickedEvent(int id) : GEEvent(GEEventType::POWERUP_PICKED), powerUpId(id) {}
};

struct GEProjectileHitEvent : GEEvent {
    int projectileId;
    int targetId;
    GEProjectileHitEvent(int pid, int tid) : GEEvent(GEEventType::PROJECTILE_HIT), projectileId(pid), targetId(tid) {}
};
