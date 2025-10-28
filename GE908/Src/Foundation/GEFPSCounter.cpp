// FpsCounter.h
#pragma once
#include <chrono>

class GEFpsCounter {
private:
    std::size_t _frameCount;
    std::chrono::steady_clock::time_point _lastTime;
    double _fps;
    double _updateIntervalSec{ 1.0 };

public:
    GEFpsCounter()
        : _frameCount(0),
        _lastTime(std::chrono::steady_clock::now()),
        _fps(0.0)
    {
    }

    void frameRendered() {
        ++_frameCount;
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = now - _lastTime;

        if (diff.count() >= _updateIntervalSec) {
            _fps = _frameCount / diff.count();
            _frameCount = 0;
            _lastTime = now;
        }
    }

    double getFps() const {
        return _fps;
    }
};
