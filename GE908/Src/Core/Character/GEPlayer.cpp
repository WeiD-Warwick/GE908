#include <algorithm>
#include "GEPlayer.h"
#include "../../Foundation/GELog.h"
#include "../../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

GEPlayer::GEPlayer() {}

GEPlayer::~GEPlayer() {}

void GEPlayer::loadData(GESaveData* saveData) {
	_saveData = saveData;

	int mapWorldWidth = _saveData->getMapTotalWidth();
	int mapWorldHeight = _saveData->getMapTotalHeight();

	// set player in the center of map
	int playerStartX = static_cast<int>((mapWorldWidth / 2.0f) - (_width / 2.0f));
	int playerStartY = static_cast<int>((mapWorldHeight / 2.0f) - (_height / 2.0f));

	setPosition(playerStartX, playerStartY);
	setMapBounds(mapWorldWidth, mapWorldHeight);
}

void GEPlayer::update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight) {
    float moveDelta = _speed * deltaTime;
    unsigned int moveAmount = static_cast<unsigned int>(max(moveDelta, 1.0f));

    if (moveUp) _originY -= moveAmount;
    if (moveDown) _originY += moveAmount;
    if (moveLeft) _originX -= moveAmount;
    if (moveRight) _originX += moveAmount;

	if (_originX < 0) _originX = 0;
	if (_originX > (_mapWidth - _width)) _originX = (_mapWidth - _width);

	if (_originY < 0) _originY = 0;
	if (_originY > (_mapHeight - _height)) _originY = (_mapHeight - _height);
}
