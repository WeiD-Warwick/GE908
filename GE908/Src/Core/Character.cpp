#include "Character.h"
#include <cmath>

void Character::moveUpdate(float deltaTime, float dirX, float dirY) {
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

void Character::takeDamage(int value) { 
    _hp = max(0, _hp - value); 
}

void Character::triggerDamageFlash(GEColor color, float duration) {
    _damageColor = color;

    _damageFlashDuration = max(0, duration);
    _damageFlashTimer = _damageFlashDuration;
}

void Character::updateCharacterState(float deltaTime) {
    if (_damageFlashTimer > 0.0f) {
        _damageFlashTimer = max(0.0f, _damageFlashTimer - deltaTime);
    }

    if (_contactDamageCooldownTimer > 0.0f) {
        _contactDamageCooldownTimer = max(0.0f, _contactDamageCooldownTimer - deltaTime);
    }
}

void Character::startContactDamageCooldown() {
    if (_contactDamageCooldownDuration <= 0.0f) {
        _contactDamageCooldownTimer = 0.0f;
    } else {
        _contactDamageCooldownTimer = _contactDamageCooldownDuration;
    }
}

void Character::draw(Window& window, const GECamera& camera) const {

    // draw character Image
    GECollisible::draw(window, camera);

    // draw HP
    drawHP(window, camera);

    // Draw Hurt flash
    drawHurt(window, camera);
}

void Character::drawHP(Window& window, const GECamera& camera) const {

    const int camX = camera.getX();
    const int camY = camera.getY();
    const int winW = window.getWidth();
    const int winH = window.getHeight();

    const int barWidth = _image.width;
    const int barHeight = 8;
    const int screenX = getOriginX() - camX;
    const int screenY = getOriginY() - camY + _image.height + 8;

    float hpRatio = (_maxHp > 0) ? static_cast<float>(_hp) / _maxHp : 0.0f;
    hpRatio = clamp(hpRatio, 0.0f, 1.0f);

    if (screenX + barWidth < 0 || screenY + barHeight < 0 || screenX >= winW || screenY >= winH)
        return;

    // clamp to window bounds
    int left = max(0, screenX);
    int right = min(winW, screenX + barWidth);
    int top = max(0, screenY);
    int bottom = min(winH, screenY + barHeight);

    // draw background
    for (int y = top; y < bottom; ++y) {
        for (int x = left; x < right; ++x) {
            window.draw(x, y, 60, 60, 60);
        }
    }

    // draw filled portion
    const int filledWidth = static_cast<int>((right - left) * hpRatio);
    for (int y = top; y < bottom; ++y) {
        for (int x = left; x < left + filledWidth; ++x) {
            if (_type == GECollisionType::Player)
                window.draw(x, y, 30, 255, 30);
            else if (_type == GECollisionType::Enemy)
                window.draw(x, y, 255, 30, 30);
        }
    }
}

void Character::drawHurt(Window& window, const GECamera& camera) const {
    if (_damageFlashTimer <= 0.0f) return;

    int camX = camera.getX();
    int camY = camera.getY();
    int winW = window.getWidth();
    int winH = window.getHeight();

    for (int dy = 0; dy < _image.height; ++dy) {
        int screenY = getOriginY() + dy - camY;
        if (screenY < 0 || screenY >= winH) continue;

        for (int dx = 0; dx < _image.width; ++dx) {
            int screenX = getOriginX() + dx - camX;
            if (screenX < 0 || screenX >= winW) continue;

            if (_image.alphaAtUnchecked(dx, dy) <= 0) continue;

            window.draw(screenX, screenY, _damageColor.r, _damageColor.g, _damageColor.b);
        }
    }
}