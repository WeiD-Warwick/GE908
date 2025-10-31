#pragma once
#include "BaseCharacter.h"
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GESaveData.h"

class GEPlayer : public BaseCharacter {
private:
	GESaveData* _saveData = nullptr;

public:
	GEPlayer();
	~GEPlayer();

	void loadData(GESaveData* saveData);
	void update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight);
};
