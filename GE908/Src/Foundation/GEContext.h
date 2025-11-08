#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Engine/CollisionSystem.h"

using namespace GamesEngineeringBase;

struct GEContext {

private:
    Window* _window = nullptr;
    CollisionSystem* _collision = nullptr;

public:

    GEContext() = default;

    explicit GEContext(Window* window, CollisionSystem* collision) : _window(window), _collision(collision) {}

    // ========== Getters ==========

    CollisionSystem* collision() { return _collision; }

    Window* window() const { return _window; }
};
