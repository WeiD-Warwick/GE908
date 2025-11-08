#pragma once
#include "Character.h"

class Enemy : public Character {
public:
    Enemy(GEPoint spawn, GEContext* ctx)
        : Character(spawn, GEColliderType::Enemy, ctx)
    {
    }

    void update(float dt) override {
        if (!isAlive()) return;
        // ???????????
        _move.move(dt, GEDirection(-1, 0));
        _collider.setOriginPosition(_move.getOriginPosition());
    }

protected:
    static void onCollideCallback(CollidableComponent* self, CollidableComponent* other) {
        if (!self || !other) return;
        if (other->getType() == GEColliderType::Player)
            printf("[Enemy] collided with Player\n");
    }

    void onDeath() override {
        printf("[Enemy] died\n");
    }
};
