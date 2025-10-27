#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GELog.h"

using namespace GamesEngineeringBase;

class BaseCharacter {

protected:
	int _x = 0;								// position x
	int _y = 0;								// position y
	int _width = 0;							// character width
	int _height = 0;						// character height
	int _speed = 200;						// movement speed
	int _hp = 0;							// hp
	bool isAlive = true;					// alive status
	int _mapWidth = 0;
	int _mapHeight = 0;

	Image* _characterImage = nullptr;		// character image

public:
	BaseCharacter() {};
	~BaseCharacter() {};

	void load(const std::string& characterImagePath) {
		_characterImage = new Image();

		if (!_characterImage->load(characterImagePath)) {
			delete _characterImage;
			_characterImage = nullptr;
			GELog::shared().error("Load player character image: " + characterImagePath + "Failed");
		}
		else {
			_width = _characterImage->width;
			_height = _characterImage->height;
			GELog::shared().info("Load player character image: " + characterImagePath + "Success");
		}
	}

	int getX() const { return _x; }
	int getY() const { return _y; }
	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	int getSpeed() const { return _speed; }
	int getHP() const { return _hp; }
	bool getIsAlive() const { return isAlive; }

	void setPosition(int x, int y) { _x = x; _y = y; }
	void setSpeed(int speed) { _speed = speed; }
	void setMapBounds(int mapWidth, int mapHeight) { _mapWidth = mapWidth; _mapHeight = mapHeight; }

	Image* getCharacterImage() const { return _characterImage; }
};

