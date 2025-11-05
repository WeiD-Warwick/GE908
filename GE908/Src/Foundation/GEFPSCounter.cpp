#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class GEFrameTimer : public Timer {
private:
    float _deltaTime = 0.0f;
    float _fps = 0.0f;
    float _accumTime = 0.0f;
    int _frameCount = 0;
    const float _updateInterval = 1.0f;

public:
    GEFrameTimer() = default;

    void beginFrame() {
        reset();
    }

    void endFrame() {
        _deltaTime = dt();  
        _frameCount++;
        _accumTime += _deltaTime;

        if (_accumTime >= _updateInterval) {
            _fps = static_cast<float>(_frameCount) / _accumTime;
            _accumTime = 0.0f;
            _frameCount = 0;
        }
    }

    float getDeltaTime() const { return _deltaTime; }
    float getFPS() const { return _fps; }
};
