#include <algorithm>
#include "GEPlayer.h"
#include "BaseCharacter.h"
#include "../../Foundation/GELog.h"
#include "../../Foundation/GESaveData.h"


GEPlayer::GEPlayer()
	: BaseCharacter(0, 0, "Src/Assets/Textures/player.png", Player) {
	_hp = 100;
	_speed = 200;
}

GEPlayer::~GEPlayer() {}

static int clamp(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}


void GEPlayer::loadData(GESaveData* saveData) {
	_saveData = saveData;

	int mapWorldWidth = _saveData->getMapTotalWidth();
	int mapWorldHeight = _saveData->getMapTotalHeight();

	int playerStartX = static_cast<int>((mapWorldWidth / 2.0f) - (_width / 2.0f));
	int playerStartY = static_cast<int>((mapWorldHeight / 2.0f) - (_height / 2.0f));

	setPosition(playerStartX, playerStartY);
	setMapBounds(mapWorldWidth, mapWorldHeight);
}

void GEPlayer::update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight) {
    float moveDelta = _speed * deltaTime;
    unsigned int moveAmount = static_cast<unsigned int>(moveDelta > 1.0f ? moveDelta : 1.0f);

    if (moveUp) _originY -= moveAmount;
    if (moveDown) _originY += moveAmount;
    if (moveLeft) _originX -= moveAmount;
    if (moveRight) _originX += moveAmount;

    int cameraWidth = _saveData->getScreenWidth();   
    int cameraHeight = _saveData->getScreenHeight();
    int mapTotalWidth = _mapWidth;
    int mapTotalHeight = _mapHeight;

    int minCenterX, maxCenterX;
    if (mapTotalWidth > cameraWidth) {
        minCenterX = cameraWidth / 2;
        maxCenterX = mapTotalWidth - cameraWidth / 2;
    }
    else {
        minCenterX = maxCenterX = mapTotalWidth / 2;
    }

    int minCenterY, maxCenterY;
    if (mapTotalHeight > cameraHeight) {
        minCenterY = cameraHeight / 2;
        maxCenterY = mapTotalHeight - cameraHeight / 2;
    }
    else {
        minCenterY = maxCenterY = mapTotalHeight / 2;
    }

    int playerHalfWidth = _width / 2;
    int playerHalfHeight = _height / 2;
    int minOriginX = minCenterX - playerHalfWidth;
    int maxOriginX = maxCenterX - playerHalfWidth;
    int minOriginY = minCenterY - playerHalfHeight;
    int maxOriginY = maxCenterY - playerHalfHeight;

    _originX = clamp(_originX, minOriginX, maxOriginX);
    _originY = clamp(_originY, minOriginY, maxOriginY);
}