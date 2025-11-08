#pragma once
#include "../Interfaces/IMovable.h"
#include "../../Foundation/GEUtility.h"

class MovableComponent : public IMovable {

private:
    GEPoint _originPoint;
    GEDirection _direction;
    float _speed = 0.0f;

public:
    MovableComponent(GEPoint originPoint = GEPoint(),
        GEDirection direction = GEDirection(),
        float speed = 0.0f)
        : _originPoint(originPoint), _direction(direction), _speed(speed) {
    }

    // ========== Interface Impl ==========
    void move(float dt, GEDirection direction) override {
        _direction = direction;
        _originPoint.x += _direction.dirX * _speed * dt;
        _originPoint.y += _direction.dirY * _speed * dt;
    }

    // ========== Setters ==========
    GEPoint getOriginPosition() const { return _originPoint; }
    GEDirection getDirection() const { return _direction; }
    float getSpeed() const { return _speed; }
    float getDirX() const { return _direction.dirX; }
    float getDirY() const { return _direction.dirY; }

    // ========== Setters ==========
    void setOriginPosition(GEPoint originPoint) { _originPoint = originPoint; }
    void setDirection(GEDirection direction) { _direction = direction; }
    void setSpeed(float speed) { _speed = speed; }

    // ========== Utils ==========
    void stop() {
        _direction.dirX = 0.0f;
        _direction.dirY = 0.0f;
    }

    bool isMoving() const {
        return (_direction.dirX != 0.0f || _direction.dirY != 0.0f);
    }
};
