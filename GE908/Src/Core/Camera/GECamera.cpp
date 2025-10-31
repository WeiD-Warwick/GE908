//
// Created by W.D. on 22/10/25
//

#include "GECamera.h"
#include <algorithm>

GECamera::GECamera() {}

GECamera::~GECamera() {}


// calculate left top world coordinates
// make the character in the center of the screen
void GECamera::followPlayer(int playerX, int playerY, int playerWidth, int playerHeight) {

	int _windowW = getWindowWidth();
	int _windowH = getWindowHeight();
	int _mapWidth = _saveData->getMapTotalWidth();
	int _mapHeight = _saveData->getMapTotalHeight();

	int targetOffsetX = playerX - (_windowW / 2) + (playerWidth / 2);
	int targetOffsetY = playerY - (_windowH / 2) + (playerHeight / 2);

	_saveData->setCameraOffsetX(std::max(0, std::min(targetOffsetX, _mapWidth - _windowW)));
	_saveData->setCameraOffsetY(std::max(0, std::min(targetOffsetY, _mapHeight - _windowH)));
}
