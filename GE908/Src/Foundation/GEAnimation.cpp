#include "GEAnimation.h"
#include "GELog.h"

GEAnimation::GEAnimation()
    : _frameCount(0)
    , _currentFrame(0)
    , _timeAccumulator(0.0f)
    , _loop(true)
    , _playing(false) {
    for (int i = 0; i < GE_ANIMATION_MAX_FRAMES; ++i) {
        _durations[i] = 0.0f;
    }
}

GEAnimation::~GEAnimation() {
    clearFrames();
}

void GEAnimation::clearFrames() {
    for (int i = 0; i < GE_ANIMATION_MAX_FRAMES; ++i) {
        _frames[i].free();
        _durations[i] = 0.0f;
    }
    _frameCount = 0;
    _currentFrame = 0;
    _timeAccumulator = 0.0f;
    _playing = false;
}

float GEAnimation::frameDurationAt(int index) const {
    if (index < 0 || index >= _frameCount) {
        return 0.0f;
    }
    float duration = _durations[index];
    if (duration <= 0.0f) {
        return 0.0001f;
    }
    return duration;
}

bool GEAnimation::loadFromFiles(const std::string filenames[], int frameCount, float frameDuration) {
    if (frameCount <= 0 || frameCount > GE_ANIMATION_MAX_FRAMES) {
        GELog::shared().warning("GEAnimation::loadFromFiles frame count out of range");
        return false;
    }

    if (frameDuration <= 0.0f) {
        frameDuration = 0.1f;
    }

    clearFrames();

    for (int i = 0; i < frameCount; ++i) {
        if (!_frames[i].load(filenames[i])) {
            GELog::shared().error("GEAnimation::loadFromFiles failed to load frame " + std::to_string(i));
            clearFrames();
            return false;
        }
        _durations[i] = frameDuration;
    }

    _frameCount = frameCount;
    _currentFrame = 0;
    _timeAccumulator = 0.0f;
    return true;
}

bool GEAnimation::loadFromFiles(const std::string filenames[], const float durations[], int frameCount) {
    if (durations == nullptr) {
        return loadFromFiles(filenames, frameCount, 0.1f);
    }

    if (frameCount <= 0 || frameCount > GE_ANIMATION_MAX_FRAMES) {
        GELog::shared().warning("GEAnimation::loadFromFiles(frame durations) frame count out of range");
        return false;
    }

    clearFrames();

    for (int i = 0; i < frameCount; ++i) {
        if (!_frames[i].load(filenames[i])) {
            GELog::shared().error("GEAnimation::loadFromFiles(frame durations) failed to load frame " + std::to_string(i));
            clearFrames();
            return false;
        }
        float duration = durations[i];
        if (duration <= 0.0f) {
            duration = 0.1f;
        }
        _durations[i] = duration;
    }

    _frameCount = frameCount;
    _currentFrame = 0;
    _timeAccumulator = 0.0f;
    return true;
}

void GEAnimation::setFrameDuration(int frameIndex, float duration) {
    if (frameIndex < 0 || frameIndex >= _frameCount) {
        return;
    }

    if (duration <= 0.0f) {
        duration = 0.1f;
    }

    _durations[frameIndex] = duration;
}

void GEAnimation::setLoop(bool shouldLoop) {
    _loop = shouldLoop;
}

void GEAnimation::play() {
    if (_frameCount == 0) {
        return;
    }
    _playing = true;
}

void GEAnimation::stop() {
    _playing = false;
}

void GEAnimation::reset() {
    _currentFrame = 0;
    _timeAccumulator = 0.0f;
}

void GEAnimation::update(float deltaTime) {
    if (!_playing || _frameCount <= 0) {
        return;
    }

    if (deltaTime <= 0.0f) {
        return;
    }

    _timeAccumulator += deltaTime;

    bool advanced = true;
    int safety = 0;
    while (advanced && _frameCount > 0 && safety < GE_ANIMATION_MAX_FRAMES) {
        advanced = false;
        float duration = frameDurationAt(_currentFrame);
        if (_timeAccumulator >= duration) {
            _timeAccumulator -= duration;
            if (_currentFrame + 1 < _frameCount) {
                _currentFrame += 1;
                advanced = true;
            }
            else if (_loop) {
                _currentFrame = 0;
                advanced = true;
            }
            else {
                _currentFrame = _frameCount - 1;
                _playing = false;
                _timeAccumulator = 0.0f;
            }
        }
        ++safety;
    }
}

void GEAnimation::draw(GEWindow& window, int topLeftX, int topLeftY) const {
    if (_frameCount <= 0) {
        return;
    }

    const Image& frame = _frames[_currentFrame];
    if (frame.data == nullptr) {
        return;
    }

    const int winW = window.getWidth();
    const int winH = window.getHeight();

    for (int dy = 0; dy < static_cast<int>(frame.height); ++dy) {
        int screenY = topLeftY + dy;
        if (screenY < 0 || screenY >= winH) {
            continue;
        }

        for (int dx = 0; dx < static_cast<int>(frame.width); ++dx) {
            int screenX = topLeftX + dx;
            if (screenX < 0 || screenX >= winW) {
                continue;
            }

            if (frame.alphaAtUnchecked(dx, dy) > 0) {
                window.draw(screenX, screenY, frame.atUnchecked(dx, dy));
            }
        }
    }
}

const Image* GEAnimation::getCurrentFrame() const {
    if (_frameCount <= 0) {
        return nullptr;
    }
    return &_frames[_currentFrame];
}

int GEAnimation::getFrameCount() const {
    return _frameCount;
}

int GEAnimation::getCurrentFrameIndex() const {
    return _currentFrame;
}

bool GEAnimation::isPlaying() const {
    return _playing;
}

bool GEAnimation::isFinished() const {
    return !_loop && !_playing && _frameCount > 0 && _currentFrame == (_frameCount - 1);
}