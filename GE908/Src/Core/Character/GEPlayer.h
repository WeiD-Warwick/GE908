#pragma once
#include <iostream>
#include "BaseCharacter.h"
#include "../../Foundation/GESaveData.h"

class GEPlayer : public BaseCharacter {

public:
	GESaveData* _saveData = nullptr;

	GEPlayer();
	~GEPlayer();

	void loadData(GESaveData* saveData);

	void update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight);
};

