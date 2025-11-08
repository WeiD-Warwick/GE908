#pragma once
#include "Character.h"
#include "../../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class Player : public Character {

public:
    Player(GEPoint spawn, GEContext* ctx)
        : Character(spawn, GEColliderType::Player, GEColliderShape::Circle, 200.f, ctx)
    {
        _render.setType(GEColliderType::Player);
        _render.showHP(true);
    }

    void update(float dt) override {
        handleInput(dt);
        Character::update(dt);
    }

    void handleInput(float dt) {
        if (!_ctx) return;

        const Window& window = *_ctx->window();
        float dirX = 0.0f;
        float dirY = 0.0f;

        if (window.keyPressed('W')) dirY -= 1.0f;
        if (window.keyPressed('S')) dirY += 1.0f;
        if (window.keyPressed('A')) dirX -= 1.0f;
        if (window.keyPressed('D')) dirX += 1.0f;

        _move.move(dt, GEDirection{ dirX , dirY });


    }

    void attack() {
        printf("[Player] Attack triggered\n");
    }

    void takeDamage(int dmg) override {
        Character::takeDamage(dmg);
        printf("[Player] Hurt: -%d HP (%d/%d)\n",
            dmg, _health.getHP(), _health.getMaxHP());
    }

    void onDeath() override {
        printf("[Player] Died\n");
    }
};
