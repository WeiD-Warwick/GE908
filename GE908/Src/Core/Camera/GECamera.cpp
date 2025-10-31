#include "GECamera.h"
#include <algorithm>

GECamera::GECamera() = default;
GECamera::~GECamera() = default;

void GECamera::load(int windowWidth, int windowHeight, int mapWidth, int mapHeight) {
	_width = windowWidth;
	_height = windowHeight;
	_mapWidth = mapWidth;
	_mapHeight = mapHeight;
	_x = 0;
	_y = 0;
}

static int clampValue(int value, int minVal, int maxVal) {
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;
	return value;
}

void GECamera::followPlayer(int playerX, int playerY, int playerWidth, int playerHeight) {
	int targetX = playerX + (playerWidth / 2) - (_width / 2);
	int targetY = playerY + (playerHeight / 2) - (_height / 2);

	if (_mapWidth > _width)
		_x = clampValue(targetX, 0, _mapWidth - _width);
	else
		_x = (_mapWidth - _width) / 2;

	if (_mapHeight > _height)
		_y = clampValue(targetY, 0, _mapHeight - _height);
	else
		_y = (_mapHeight - _height) / 2;
}

void GECamera::setMapBounds(int mapWidth, int mapHeight) {
	_mapWidth = mapWidth;
	_mapHeight = mapHeight;
}

int GECamera::worldToScreenX(int worldX) const {
	return worldX - _x;
}

int GECamera::worldToScreenY(int worldY) const {
	return worldY - _y;
}

int GECamera::screenToWorldX(int screenX) const {
	return screenX + _x;
}

int GECamera::screenToWorldY(int screenY) const {
	return screenY + _y;
}
