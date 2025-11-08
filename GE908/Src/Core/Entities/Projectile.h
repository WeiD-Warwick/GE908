#pragma once
#include "Entity.h"
#include "../Components/MovableComponent.h"
#include "../Components/CollidableComponent.h"

class Projectile : public Entity {
private:
    MovableComponent    _move;
    CollidableComponent _collider;
    float _lifetime = 3.0f;

public:
    Projectile(GEPoint spawn, GEDirection dir, GEContext* ctx)
        : Entity(ctx),
        _move(spawn, dir, 400.0f),
        _collider(spawn, GESize(8, 8),
            GEColliderType::Projectile,
            GEColliderShape::Circle,
            onProjectileCollide,
            ctx)
    {
        setMovable(&_move);
        setCollidable(&_collider);
    }

    void update(float dt) override {
        _move.move(dt, _move.getDirection());
        _collider.setOriginPosition(_move.getOriginPosition());

        _lifetime -= dt;
        if (_lifetime <= 0)
            onExpire();
    }

private:
    static void onProjectileCollide(CollidableComponent* self, CollidableComponent* other) {
        if (!self || !other) return;
        printf("[Projectile] hit something of type %d\n", (int)other->getType());
    }

    void onExpire() {
        printf("[Projectile] expired\n");
    }
};
