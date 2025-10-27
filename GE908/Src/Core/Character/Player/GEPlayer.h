#pragma once
#include <iostream>
#include "../../../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class GEPlayer {

protected:
	int _x = 0;					// position x
	int _y = 0;					// position y

	int _width = 0;				// character width
	int _height = 0;			// character height

	int _speed = 200;				// movement speed

	int _hp = 0;					// hp
	int _mp = 0;					// mp

	bool isAlive = true;				// alive status

	Image* _characterImage = nullptr;		// character image

public:
	GEPlayer();
	~GEPlayer();

	void load(const std::string& characterImagePath);

	void update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight);

	int getX() const { return _x; }
	int getY() const { return _y; }
	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	int getSpeed() const { return _speed; }
	bool getIsAlive() const { return isAlive; }
	int getHP() const { return _hp; }
	int getMP() const { return _mp; }

	void setPosition(int x, int y) { _x = x; _y = y; }

	Image* getCharacterImage() const { return _characterImage; }
};

