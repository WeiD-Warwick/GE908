#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Foundation/GECamera.h"
#include "GEDebug.h"

using namespace GamesEngineeringBase;

enum class GECollisionType {
	None,
	Player,
	Enemy,
	Water,
	Fire,
	Projectile,
	PowerUp,
};
	
class GECollisible {

protected:
	float _centerX = 0.0f;
	float _centerY = 0.0f;
	GECollisionType _type = GECollisionType::None;
	Image _image;

public:

	GECollisible(const std::string& filename, GECollisionType collisionType)
		: _type(collisionType) {
		_image.load(filename);
	}

	virtual ~GECollisible() = default;

	float getOriginX() const { return _centerX - _image.width / 2.0f;}
	float getOriginY() const { return _centerY - _image.height / 2.0f;}
	float getCenterX() const { return _centerX;}
	float getCenterY() const { return _centerY;}
	int getWidth() const { return _image.width;}
	int getHeight() const { return _image.height;}
	int getCollisionRadius() const { return _image.width / 2;}
	GECollisionType getCollisionType() const { return _type; }

    void setCenter(float centerX, float centerY) { _centerX = centerX; _centerY = centerY; }

    // draw collider's image
	virtual void draw(Window& window, const GECamera& camera) const {
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

				if (_image.alphaAtUnchecked(dx, dy) > 0)
					window.draw(screenX, screenY, _image.atUnchecked(dx, dy));
			}
		}

		drawCollisionBoxIfNeeded(window, camera);
	}

    // draw collider's collider box
    virtual void drawCollisionBoxIfNeeded(Window& window, const GECamera& camera) const {

        if (!GEDebug::shared().needDrawCollisionBounds() || _type == GECollisionType::None) return;

        int camX = camera.getX();
        int camY = camera.getY();
        int winW = window.getWidth();
        int winH = window.getHeight();

        if (_type == GECollisionType::Water || _type == GECollisionType::Fire) {
            int left = getOriginX() - camX;
            int top = getOriginY() - camY;
            int right = left + _image.width;
            int bottom = top + _image.height;

            if (right <= 0 || bottom <= 0 || left >= winW || top >= winH) {

            }
            else {
                if (left < 0)   left = 0;
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
                        if (yT >= 0 && yT < winH)
                            window.draw(x, yT, 0, 0, 255);
                        if (yB >= 0 && yB < winH)
                            window.draw(x, yB, 0, 0, 255);
                    }

                    for (int y = yT; y <= yB; ++y) {
                        if (xL >= 0 && xL < winW)
                            window.draw(xL, y, 0, 0, 255);
                        if (xR >= 0 && xR < winW)
                            window.draw(xR, y, 0, 0, 255);
                    }
                }
            }
        }
        else {
            const int radius = getCollisionRadius();
            const int cx = getOriginX() + _image.width / 2 - camX;
            const int cy = getOriginY() + _image.height / 2 - camY;
            const int r2 = radius * radius;

            for (int dx = -radius; dx <= radius; ++dx) {
                const int rem = r2 - dx * dx;
                if (rem < 0) continue;
                const int dy = static_cast<int>(sqrtf(static_cast<float>(rem)));

                const int x = cx + dx;
                if (x < 0 || x >= winW) continue;

                const int y1 = cy + dy;
                const int y2 = cy - dy;
                if (y1 >= 0 && y1 < winH)
                    window.draw(x, y1, 255, 0, 0);
                if (y2 >= 0 && y2 < winH)
                    window.draw(x, y2, 255, 0, 0);
            }
        }

    }
   
    // check if this collider collide with another right now
    bool collide(const GECollisible& other) const {
        if (_type == GECollisionType::Water && other._type == GECollisionType::Water) {
            return false;
        }

        // circle <-> reatangle
        if (_type == GECollisionType::Water || other._type == GECollisionType::Water) {
            const GECollisible& rect = (_type == GECollisionType::Water) ? *this : other;
            const GECollisible& circle = (_type == GECollisionType::Water) ? other : *this;

            float rectLeft = rect.getOriginX();
            float rectTop = rect.getOriginY();
            float rectRight = rectLeft + rect.getWidth();
            float rectBottom = rectTop + rect.getHeight();

            return circleRectCollision(
                circle.getCenterX(),
                circle.getCenterY(),
                static_cast<float>(circle.getCollisionRadius()),
                rectLeft, rectTop, rectRight, rectBottom
            );
        }

        // circle <-> circle
        float dx = _centerX - other._centerX;
        float dy = _centerY - other._centerY;
        float distSquared = dx * dx + dy * dy;
        float combinedRadius = getCollisionRadius() + other.getCollisionRadius();
        return distSquared <= combinedRadius * combinedRadius;
    }

    // check if this collider would collide with another when placed at position (cx, cy), without actually moving it
    bool collideAt(float cx, float cy, const GECollisible& other) const {
        const float aHalfW = _image.width / 2.0f;
        const float aHalfH = _image.height / 2.0f;

        const float bCx = other.getCenterX();
        const float bCy = other.getCenterY();
        const float bHalfW = other.getWidth() / 2.0f;
        const float bHalfH = other.getHeight() / 2.0f;

        if (_type == GECollisionType::Water && other._type == GECollisionType::Water) {
            return false;
        }

        if (_type == GECollisionType::Water) {
            const float left = cx - aHalfW;
            const float right = cx + aHalfW;
            const float top = cy - aHalfH;
            const float bottom = cy + aHalfH;
            return circleRectCollision(
                bCx, bCy,
                static_cast<float>(other.getCollisionRadius()),
                left, top, right, bottom
            );
        }

        if (other._type == GECollisionType::Water) {
            const float left = bCx - bHalfW;
            const float right = bCx + bHalfW;
            const float top = bCy - bHalfH;
            const float bottom = bCy + bHalfH;
            return circleRectCollision(
                cx, cy,
                static_cast<float>(getCollisionRadius()),
                left, top, right, bottom
            );
        }


        const float dx = cx - bCx;
        const float dy = cy - bCy;
        const float distSq = dx * dx + dy * dy;
        const float r = static_cast<float>(getCollisionRadius() + other.getCollisionRadius());
        return distSq <= r * r;
    }
};