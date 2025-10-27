#pragma once
#include <iostream>
#include "../BaseCharacter.h"

class GEPlayer : public BaseCharacter {

public:
	GEPlayer();
	~GEPlayer();

	void update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight);
};

