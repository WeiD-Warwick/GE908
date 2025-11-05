#include "BaseCharacter.h"

void BaseCharacter::moveUpdate(float deltaTime, float dirX, float dirY) {
    if (dirX == 0 && dirY == 0) return;

    // normalize direction
    float len = std::sqrt(dirX * dirX + dirY * dirY);
    if (len <= 0.0001f) return;
    dirX /= len;
    dirY /= len;

    float moveDelta = _speed * deltaTime;
    float rawDeltaX = dirX * moveDelta;
    float rawDeltaY = dirY * moveDelta;

    _accumX += rawDeltaX;
    _accumY += rawDeltaY;

    float deltaX = std::floor(std::abs(_accumX)) * ((_accumX >= 0) ? 1 : -1);
    float deltaY = std::floor(std::abs(_accumY)) * ((_accumY >= 0) ? 1 : -1);

    _accumX -= deltaX;
    _accumY -= deltaY;

    if (deltaX == 0 && deltaY == 0) return;

    float newX = getCenterX();
    float newY = getCenterY();
    const int COLLISION_MARGIN = 2;

    // X
    if (deltaX != 0) {
        float nextX = newX + deltaX;
        if (isBlockedAt(nextX, getCenterY())) {
            int startX = static_cast<int>(getCenterX());
            int endX = static_cast<int>(nextX);
            int backStep = (deltaX > 0) ? 1 : -1;
            for (int x = startX; x != endX; x += backStep) {
                if (isBlockedAt(x, getCenterY())) {
                    newX = x - backStep * COLLISION_MARGIN;
                    break;
                }
                newX = x;
            }
        }
        else {
            newX = nextX;
        }
    }

    // Y
    if (deltaY != 0) {
        float nextY = newY + deltaY;
        if (isBlockedAt(getCenterX(), nextY)) {
            int startY = static_cast<int>(getCenterY());
            int endY = static_cast<int>(nextY);
            int backStep = (deltaY > 0) ? 1 : -1;
            for (int y = startY; y != endY; y += backStep) {
                if (isBlockedAt(getCenterX(), y)) {
                    newY = y - backStep * COLLISION_MARGIN;
                    break;
                }
                newY = y;
            }
        }
        else {
            newY = nextY;
        }
    }

    // check boundary
    applyMovementBounds(newX, newY);
    // set position
    setCenter(newX, newY);
}

void BaseCharacter::takeDamage(int value) {
    int newValue = _hp - value;
    _hp = newValue > 0 ? newValue : 0;
}