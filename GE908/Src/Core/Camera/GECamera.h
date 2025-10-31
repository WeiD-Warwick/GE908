//
// Created by W.D. on 22/10/25
//

#pragma once
#include "../../Foundation/GESaveData.h"

class GECamera {
private:
	GESaveData* _saveData = nullptr;

public:

	GECamera();
	~GECamera();

	void loadData(GESaveData* saveData) { _saveData = saveData; }

    void followPlayer(int playerX, int playerY, int playerWidth, int playerHeight);
	int getOffsetX() const { return _saveData->getCameraOffsetX(); }
	int getOffsetY() const { return _saveData->getCameraOffsetY(); }

	int getWindowWidth() { return _saveData->getWindowWidth(); }
	int getWindowHeight() { return _saveData->getWindowHeight(); }
};