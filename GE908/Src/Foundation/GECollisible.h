#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Core/Camera/GECamera.h"

#define SHOW_COLLISION_CIRCLE true

class GEWindow;

using namespace GamesEngineeringBase;

enum GECollisionType {
	None,
	Player,
};

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
	int getCollisionRadius() const { return image.width / 3; }

	void draw(GEWindow& window, const GECamera& camera);

	// collide detect
	bool collide(const GECollisible& other) const;
};