#include "GECollisible.h"
#include "GEWindow.h"

void GECollisible::draw(GEWindow& window) {
    for (unsigned int dy = 0; dy < image.height; dy++) {
        if (_originY + dy > 0 && _originY + dy < window.getHeight()) {
            for (unsigned int dx = 0; dx < image.width; dx++) {
                if (_originX + dx > 0 && _originX + dx < window.getWidth() &&
                    image.alphaAtUnchecked(dx, dy) > 0) {
                    window.draw(_originX + dx, _originY + dy, image.atUnchecked(dx, dy));
                }
            }
        }
    }
    if (SHOW_COLLISION_CIRCLE) drawCollisionCircle(window);
}

void GECollisible::drawCollisionCircle(GEWindow& window) const {
    int radius = getCollisionRadius();
    int cx = _originX + image.width / 2;
    int cy = _originY + image.height / 2;
    int r2 = radius * radius;
    for (int dx = -radius; dx <= radius; dx++) {
        int dy = static_cast<int>(sqrtf(static_cast<float>(r2 - dx * dx)));
        if (cx + dx >= 0 && cx + dx < window.getWidth()) {
            if (cy + dy >= 0 && cy + dy < window.getHeight())
                window.draw(cx + dx, cy + dy, 255, 255, 0);
            if (cy - dy >= 0 && cy - dy < window.getHeight())
                window.draw(cx + dx, cy - dy, 255, 255, 0);
        }
    }
}

bool GECollisible::collide(const GECollisible& other) const {
    int dx = (_originX + image.width / 2) - (other._originX + other.image.width / 2);
    int dy = (_originY + image.height / 2) - (other._originY + other.image.height / 2);
    int distSquared = dx * dx + dy * dy;
    int combinedRadius = getCollisionRadius() + other.getCollisionRadius();
    return distSquared < combinedRadius * combinedRadius;
}
