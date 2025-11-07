#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Foundation/GECamera.h"

using namespace GamesEngineeringBase;

#define SHOW_COLLISION_CIRCLE true

enum class GECollisionType {
	None,
	Player,
	Enemy,
	Water,
	Fire,
	Projectile,
	PowerUp,
};

static bool circleRectCollision(float circleX, float circleY, float radius,
	float rectLeft, float rectTop, float rectRight, float rectBottom) {
	float closestX = clamp(circleX, rectLeft, rectRight);
	float closestY = clamp(circleY, rectTop, rectBottom);

	float dx = circleX - closestX;
	float dy = circleY - closestY;

	return (dx * dx + dy * dy) <= (radius * radius);
}
	
class GECollisible {

protected:
	float _centerX = 0.0f;
	float _centerY = 0.0f;
	GECollisionType _type;
	Image _image;

public:

	GECollisible(const std::string& filename, GECollisionType collisionType)
		: _type(collisionType) {
		_image.load(filename);
	}

	virtual ~GECollisible() = default;

	virtual void update(GamesEngineeringBase::Window& canvas, float dt) {}

	void setCenter(float centerX, float centerY) { _centerX = centerX; _centerY = centerY;}
	float getOriginX() const { return _centerX - _image.width / 2.0f;}
	float getOriginY() const { return _centerY - _image.height / 2.0f;}
	float getCenterX() const { return _centerX;}
	float getCenterY() const { return _centerY;}
	int getWidth() const { return _image.width;}
	int getHeight() const { return _image.height;}
	int getCollisionRadius() const { return _image.width / 2;}
	GECollisionType getCollisionType() const { return _type; }

	void draw(Window& window, const GECamera& camera);
	bool collide(const GECollisible& other) const;
	bool collideAt(float x, float y, const GECollisible& other) const;
};