#include "GECollisible.h"
#include "GEWindow.h"

void GECollisible::draw(GEWindow& window, const GECamera& camera) {
    int camX = camera.getX();
    int camY = camera.getY();
    int winW = window.getWidth();
    int winH = window.getHeight();

    for (int dy = 0; dy < image.height; dy++) {
        int screenY = _originY + (int)dy - camY;
        if (screenY < 0 || screenY >= winH) continue;

        for (int dx = 0; dx < image.width; dx++) {
            int screenX = _originX + (int)dx - camX;
            if (screenX < 0 || screenX >= winW) continue;

            if (image.alphaAtUnchecked(dx, dy) > 0)
                window.draw(screenX, screenY, image.atUnchecked(dx, dy));
        }
    }

    if (SHOW_COLLISION_CIRCLE && _type != None) {
        const int camX = camera.getX();
        const int camY = camera.getY();
        const int winW = window.getWidth();
        const int winH = window.getHeight();

        if (_type == Water) {
            int left = _originX - camX;
            int top = _originY - camY;
            int right = left + image.width;   
            int bottom = top + image.height; 


            if (right <= 0 || bottom <= 0 || left >= winW || top >= winH) {

            }
            else {
                if (left < 0)    left = 0;
                if (top < 0)    top = 0;
                if (right > winW) right = winW;
                if (bottom > winH) bottom = winH;

                const int width = right - left;
                const int height = bottom - top;
                if (width > 0 && height > 0) {
                    const int xL = left;
                    const int xR = right - 1; 
                    const int yT = top;
                    const int yB = bottom - 1;

                    for (int x = xL; x <= xR; ++x) {
                        if (yT >= 0 && yT < winH) window.draw(x, yT, 0, 0, 255);
                        if (yB >= 0 && yB < winH) window.draw(x, yB, 0, 0, 255);
                    }
 
                    for (int y = yT; y <= yB; ++y) {
                        if (xL >= 0 && xL < winW) window.draw(xL, y, 0, 0, 255);
                        if (xR >= 0 && xR < winW) window.draw(xR, y, 0, 0, 255);
                    }
                }
            }
        }
        else {
            const int radius = getCollisionRadius();
            const int cx = _originX + image.width / 2 - camX;
            const int cy = _originY + image.height / 2 - camY;
            const int r2 = radius * radius;

            for (int dx = -radius; dx <= radius; ++dx) {
                const int rem = r2 - dx * dx;
                if (rem < 0) continue;
                const int dy = static_cast<int>(sqrtf(static_cast<float>(rem)));

                const int x = cx + dx;
                if (x < 0 || x >= winW) continue;

                const int y1 = cy + dy;
                const int y2 = cy - dy;
                if (y1 >= 0 && y1 < winH) window.draw(x, y1, 255, 0, 0);
                if (y2 >= 0 && y2 < winH) window.draw(x, y2, 255, 0, 0);
            }
        }
    }
}

bool GECollisible::collide(const GECollisible& other) const {
    // water -> reatangle
    if (_type == Water || other._type == Water) {
        int leftA = _originX;
        int rightA = _originX + image.width;
        int topA = _originY;
        int bottomA = _originY + image.height;

        int leftB = other._originX;
        int rightB = other._originX + other.image.width;
        int topB = other._originY;
        int bottomB = other._originY + other.image.height;

        bool overlapX = (leftA < rightB) && (rightA > leftB);
        bool overlapY = (topA < bottomB) && (bottomA > topB);

        return overlapX && overlapY;
    }

    // circle
    int dx = (_originX + image.width / 2) - (other._originX + other.image.width / 2);
    int dy = (_originY + image.height / 2) - (other._originY + other.image.height / 2);
    int distSquared = dx * dx + dy * dy;
    int combinedRadius = getCollisionRadius() + other.getCollisionRadius();
    return distSquared < combinedRadius * combinedRadius;
}

bool GECollisible::collideAt(int x, int y, const GECollisible& other) const {
    if (_type == Water || other._type == Water) {
        int leftA = x;
        int rightA = x + image.width;
        int topA = y;
        int bottomA = y + image.height;

        int leftB = other._originX;
        int rightB = other._originX + other.image.width;
        int topB = other._originY;
        int bottomB = other._originY + other.image.height;

        bool overlapX = (leftA < rightB) && (rightA > leftB);
        bool overlapY = (topA < bottomB) && (bottomA > topB);

        return overlapX && overlapY;
    }

    int dx = (x + image.width / 2) - (other._originX + other.image.width / 2);
    int dy = (y + image.height / 2) - (other._originY + other.image.height / 2);
    int distSquared = dx * dx + dy * dy;
    int combinedRadius = getCollisionRadius() + other.getCollisionRadius();
    return distSquared < combinedRadius * combinedRadius;
}
