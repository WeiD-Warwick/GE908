#include "BaseCharacter.h"
#include <cmath>

void BaseCharacter::moveUpdate(float deltaTime, float dirX, float dirY) {
    if (dirX == 0 && dirY == 0) return;

    // normalize
    float len = std::sqrt(dirX * dirX + dirY * dirY);
    if (len <= 0.0001f) return;
    dirX /= len;
    dirY /= len;

    // move amount
    float moveDelta = _speed * deltaTime;
    float deltaX = dirX * moveDelta;
    float deltaY = dirY * moveDelta;

    float newX = getCenterX();
    float newY = getCenterY();

    float nextX = newX + deltaX;
    float nextY = newY + deltaY;

    if (!isBlockedAt(nextX, nextY)) {
        newX = nextX;
        newY = nextY;
    }
    else {
        if (!isBlockedAt(nextX, newY)) newX = nextX;
        if (!isBlockedAt(newX, nextY)) newY = nextY;
    }

    applyMovementBounds(newX, newY);
    setCenter(newX, newY);

}

void BaseCharacter::takeDamage(int value) { _hp = max(0, _hp - value); }