#pragma once
#include "../../Foundation/GEContext.h"
#include "../Components/MovableComponent.h"
#include "../Components/CollidableComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/RenderableComponent.h"

class Character {
protected:
    GEContext* _ctx = nullptr;

    MovableComponent    _move;
    CollidableComponent _collider;
    HealthComponent     _health;
    RenderableComponent _render;

public:
    Character(GEPoint spawn,
              GEColliderType type,
              GEColliderShape shape,
              float speed,
              GEContext* ctx)
        : _ctx(ctx),
          _move(spawn, GEDirection(), speed),
          _collider(spawn, GESize(32, 32), type, shape, ctx),
          _health(100, 100) {
    }

    virtual ~Character() = default;

    virtual void update(float dt) {
        if (!_health.isAlive()) return;

        _move.move(dt, _move.getDirection());
        _collider.setOriginPosition(_move.getOriginPosition());
        _render.setOrigin(_move.getOriginPosition());
        _render.setHpRatio(_health.getHP() / (float)_health.getMaxHP());
        _render.updateRender(dt);

        if (!_health.isAlive()) handleDeath();
    }

    virtual void render(Window& window, const GECamera& camera) const {
        _render.render(window, camera);
    }

    // ========================
    // ?????
    // ========================
    virtual void takeDamage(int dmg) {
        _health.takeDamage(dmg);
        _render.triggerFlash(GEColor(255, 60, 60), 0.15f);
        if (!_health.isAlive())
            handleDeath();
    }

    virtual void heal(int value) {
        _health.heal(value);
        _render.setHpRatio(_health.getHP() / (float)_health.getMaxHP());
    }

    // ========================
    // ????
    // ========================
    virtual void handleDeath() {
        if (_health.isAlive()) {
            onDeath();
        }
    }

    virtual void onDeath() {
        printf("[Character] %p died\n", this);
    }

    // ========================
    // ????
    // ========================
    GEPoint getPosition() const { return _move.getOriginPosition(); }
    bool isAlive() const { return _health.isAlive(); }

    MovableComponent& move() { return _move; }
    CollidableComponent& collider() { return _collider; }
    HealthComponent& health() { return _health; }
    RenderableComponent& renderable() { return _render; }
};
