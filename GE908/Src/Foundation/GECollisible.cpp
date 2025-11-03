#include "GECollisible.h"
#include "GEWindow.h"

void GECollisible::draw(GEWindow& window, const GECamera& camera) {
    int camX = camera.getX();
    int camY = camera.getY();
    int winW = window.getWidth();
    int winH = window.getHeight();

    for (int dy = 0; dy < image.height; dy++) {
        int sy = _originY + (int)dy - camY;
        if (sy < 0 || sy >= winH) continue;

        for (int dx = 0; dx < image.width; dx++) {
            int sx = _originX + (int)dx - camX;
            if (sx < 0 || sx >= winW) continue;

            if (image.alphaAtUnchecked(dx, dy) > 0)
                window.draw(sx, sy, image.atUnchecked(dx, dy));
        }
    }

    if (SHOW_COLLISION_CIRCLE && _type != None) {
        int radius = getCollisionRadius();
        int cx = _originX + image.width / 2 - camX;
        int cy = _originY + image.height / 2 - camY;
        int r2 = radius * radius;
        for (int dx = -radius; dx <= radius; dx++) {
            int dy = static_cast<int>(sqrtf(static_cast<float>(r2 - dx * dx)));
            if (cx + dx >= 0 && cx + dx < winW) {
                if (cy + dy >= 0 && cy + dy < winH)
                    window.draw(cx + dx, cy + dy, 255, 0, 0);
                if (cy - dy >= 0 && cy - dy < winH)
                    window.draw(cx + dx, cy - dy, 255, 0, 0);
            }
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
