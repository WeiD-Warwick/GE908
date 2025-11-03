#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GELog.h"
#include "../../Foundation/GEWindow.h"

using namespace GamesEngineeringBase;

class BaseCharacter : public GECollisible {

protected:
	int _width = 0;
	int _height = 0;
	int _speed = 100;
	int _hp = 0;
	int _mapWidth = 0;
	int _mapHeight = 0;

public:
	BaseCharacter(int x = 0, int y = 0, const std::string& filename = "", GECollisionType type = None)
		: GECollisible(x, y, filename, type) {
		_width = image.width;
		_height = image.height;
	}

	virtual ~BaseCharacter() {}

	void load(const std::string& characterImagePath) {
		if (!image.load(characterImagePath)) {
			GELog::shared().error("Load character image failed: " + characterImagePath);
		}
		else {
			_width = image.width;
			_height = image.height;
			GELog::shared().info("Loaded character image: " + characterImagePath);
		}
	}

	int getX() const { return _originX; }
	int getY() const { return _originY; }
	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	int getSpeed() const { return _speed; }
	int getHP() const { return _hp; }
	bool isAlive() const { return _hp > 0; }

	void setPosition(int x, int y) { _originX = x; _originY = y; }
	void setSpeed(int speed) { _speed = speed; }
	void setMapBounds(int mapWidth, int mapHeight) { _mapWidth = mapWidth; _mapHeight = mapHeight; }
};
