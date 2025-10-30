#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "GEWindow.h"
#define SHOW_COLLISION_CIRCLE true

using namespace GamesEngineeringBase;

enum GECollisionType { None, Player, };

class GECollisible {

protected:
	int _originX;
	int _originY;

	GECollisionType _type;
	Image image;

public:

	GECollisible(int originX, int originY, const std::string& filename, GECollisionType collisionType)
		: _originX(originX), _originY(originY), _type(collisionType) {
		image.load(filename);
	}

	virtual ~GECollisible() = default;

	virtual void update(GamesEngineeringBase::Window& canvas, float dt) {}

	int getOriginX() const { return _originX; }
	int getOriginY() const { return _originY; }
	int getImageWidth() const { return image.width; }
	int getImageHeight() const { return image.height; }

	void draw(GEWindow& window) {
		for (unsigned int dy = 0; dy < image.height; dy++) {
			if (_originY + dy > 0 && _originY + dy < window.getHeight()) {
				for (unsigned int dx = 0; dx < image.width; dx++) {
					if (_originX + dx> 0 && _originX + dx < window.getWidth() && image.alphaAtUnchecked(dx, dy) > 0) {
						window.draw(_originX + dx, _originY + dy, image.atUnchecked(dx, dy));
					}
				}
			}
		}

		if (SHOW_COLLISION_CIRCLE)
			drawCollisionCircle(window);
	}

	void drawCollisionCircle(GEWindow& window) const {
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

	int getCollisionRadius() const {
		return image.width / 3;
	}

	// collide detect
	bool collide(const GECollisible& other) const {
		int dx = (_originX + image.width / 2) - (other._originX + other.image.width / 2);
		int dy = (_originY + image.height / 2) - (other._originY + other.image.height / 2);
		int distSquared = dx * dx + dy * dy;
		int combinedRadius = getCollisionRadius() + other.getCollisionRadius();
		return distSquared < combinedRadius * combinedRadius;
	}
};