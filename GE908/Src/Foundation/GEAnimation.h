#pragma once
#include <string>
#include "../../ThirdParty/GamesEngineeringBase.h"

#define GE_ANIMATION_MAX_FRAMES 32

using namespace GamesEngineeringBase;

class GEAnimation {
private:
    Image _frames[GE_ANIMATION_MAX_FRAMES];
    float _durations[GE_ANIMATION_MAX_FRAMES];
    int _frameCount;
    int _currentFrame;
    float _timeAccumulator;
    bool _loop;
    bool _playing;

    void clearFrames();
    float frameDurationAt(int index) const;

public:
    GEAnimation();
    ~GEAnimation();

    bool loadFromFiles(const std::string filenames[], int frameCount, float frameDuration);
    bool loadFromFiles(const std::string filenames[], const float durations[], int frameCount);

    void setFrameDuration(int frameIndex, float duration);
    void setLoop(bool shouldLoop);
    void play();
    void stop();
    void reset();

    void update(float deltaTime);
    void draw(Window& window, int topLeftX, int topLeftY) const;

    const Image* getCurrentFrame() const;
    int getFrameCount() const;
    int getCurrentFrameIndex() const;
    bool isPlaying() const;
    bool isFinished() const;
};