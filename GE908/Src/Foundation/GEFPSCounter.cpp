// FpsCounter.h
#pragma once

#include <chrono>

class GEFpsCounter {
private:
    std::size_t m_frameCount;
    std::chrono::steady_clock::time_point m_lastTime;
    double m_fps;
    double m_updateIntervalSec{ 1.0 };

public:
    GEFpsCounter()
        : m_frameCount(0),
        m_lastTime(std::chrono::steady_clock::now()),
        m_fps(0.0)
    {
    }

    void frameRendered() {
        ++m_frameCount;
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = now - m_lastTime;

        if (diff.count() >= m_updateIntervalSec) {
            // ?? FPS
            m_fps = m_frameCount / diff.count();
            // ??
            m_frameCount = 0;
            m_lastTime = now;
        }
    }

    double getFps() const {
        return m_fps;
    }
};
