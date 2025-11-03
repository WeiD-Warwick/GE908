#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Core/Camera/GECamera.h"

#define SHOW_COLLISION_CIRCLE true

class GEWindow;

using namespace GamesEngineeringBase;

enum GECollisionType {
	None,
	Player,
	Enemy,
	Water,
};

class GECollisible {

protected:
	int _originX = 0;
	int _originY = 0;
	GECollisionType _type;
	Image image;

public:

	GECollisible(const std::string& filename, GECollisionType collisionType)
		: _type(collisionType) {
		image.load(filename);
	}

	virtual ~GECollisible() = default;

	virtual void update(GamesEngineeringBase::Window& canvas, float dt) {}

	void setPosition(int x, int y) { _originX = x; _originY = y; }
	int getOriginX() const { return _originX; }
	int getOriginY() const { return _originY; }
	int getImageWidth() const { return image.width; }
	int getImageHeight() const { return image.height; }
	int getCollisionRadius() const { return image.width / 2; }

	void draw(GEWindow& window, const GECamera& camera);
	bool collide(const GECollisible& other) const;
};