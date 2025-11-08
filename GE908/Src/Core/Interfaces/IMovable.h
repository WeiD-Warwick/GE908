#pragma once

class IMovable {
public:
    virtual void move(float dt, GEDirection direction) = 0;
    virtual ~IMovable() = default;
};