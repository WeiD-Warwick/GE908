#pragma once

#include "../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class GEDebugState {

private:

    bool _lastFramePressedCollision = false;
    bool _lastFramePressedAOE = false;
    bool _drawCollisionBounds = false;
    bool _drawAOEIndicators = false;

    GEDebugState() = default;

    void updateCollisionToggle(Window& window) {
        bool pressed = window.keyPressed('1');
        if (pressed && !_lastFramePressedCollision) 
            _drawCollisionBounds = !_drawCollisionBounds;
        _lastFramePressedCollision = pressed;
    }

    void updateAOEIndiactorsToggle(Window& window) {
        bool pressed = window.keyPressed('2');
        if (pressed && !_lastFramePressedAOE) 
            _drawAOEIndicators = !_drawAOEIndicators;
        _lastFramePressedAOE = pressed;
    }

public:
    static GEDebugState& shared() {
        static GEDebugState state;
        return state;
    }

    void updateFromInput(Window& window) {
        updateCollisionToggle(window);
        updateAOEIndiactorsToggle(window);
    }

    bool drawCollisionBounds() const { return _drawCollisionBounds; }
    bool drawSkillIndicators() const { return _drawAOEIndicators; }
};