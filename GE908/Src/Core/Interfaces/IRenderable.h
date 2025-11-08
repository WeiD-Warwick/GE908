#pragma once
#include "../../Foundation/GECamera.h"
#include "../../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class IRenderable {
public:
    // ?????????????????
    virtual void updateRender(float deltaTime) = 0;

    // ?????????
    virtual void render(Window& window, const GECamera& camera) const = 0;

    virtual ~IRenderable() = default;
};
