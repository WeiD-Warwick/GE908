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

static float clampValue(float value, float minVal, float maxVal) {
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;
	return value;
}

void GECamera::followPlayer(float playerX, float playerY, int playerWidth, int playerHeight) {
	float targetX = playerX + (playerWidth / 2.0f) - (_width / 2.0f);
	float targetY = playerY + (playerHeight / 2.0f) - (_height / 2.0f);

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
