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
	int _speed = 70;
	int _hp = 0;
	int _mapWidth = 0;
	int _mapHeight = 0;

public:
	BaseCharacter(const std::string& filename = "", GECollisionType type = None)
		: GECollisible(filename, type) {
		_width = _image.width;
		_height = _image.height;
	}

	virtual ~BaseCharacter() {}

	void load(const std::string& characterImagePath) {
		if (!_image.load(characterImagePath)) {
			GELog::shared().error("Load character image failed: " + characterImagePath);
		}
		else {
			_width = _image.width;
			_height = _image.height;
			GELog::shared().info("Loaded character image: " + characterImagePath);
		}
	}

	int getSpeed() const { return _speed; }
	int getHP() const { return _hp; }
	bool isAlive() const { return _hp > 0; }
	void setSpeed(int speed) { _speed = speed; }
	void setMapBounds(int mapWidth, int mapHeight) { _mapWidth = mapWidth; _mapHeight = mapHeight; }

	void takeDamage(int value) {
		int newValue = _hp - value;
		_hp = newValue > 0 ? newValue : 0;
	}
};
