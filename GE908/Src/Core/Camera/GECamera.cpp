//
// Created by W.D. on 22/10/25
//

#include "GECamera.h"
#include <algorithm>

GECamera::GECamera(int screenW, int screenH)
	: _offsetX(0), _offsetY(0), _screenW(screenW), _screenH(screenH) {
}

GECamera::~GECamera() {}


// calculate left top world coordinates
// make the character in the center of the screen
void GECamera::followPlayer(int playerX, int playerY, int playerWidth, int playerHeight) {

	int targetOffsetX = playerX - (_screenW / 2) + (playerWidth / 2);
	int targetOffsetY = playerY - (_screenH / 2) + (playerHeight / 2);

	// border check, pick valided one
	_offsetX = std::max(0, std::min(targetOffsetX, _mapColCount - _screenW));
	_offsetY = std::max(0, std::min(targetOffsetY, _mapRowCount - _screenH));
}

void GECamera::setMapBounds(int mapWidth, int mapHeight) {
	_mapColCount = mapWidth;
	_mapRowCount = mapHeight;
}
