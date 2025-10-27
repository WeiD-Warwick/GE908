#include <algorithm>
#include "GEPlayer.h"
#include "../../../Foundation/GELog.h"
#include "../../../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

GEPlayer::GEPlayer() {}

GEPlayer::~GEPlayer() {}

void GEPlayer::update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight) {
    float moveDelta = _speed * deltaTime;
    unsigned int moveAmount = static_cast<unsigned int>(max(moveDelta, 1.0f));

    if (moveUp) _y -= moveAmount;
    if (moveDown) _y += moveAmount;
    if (moveLeft) _x -= moveAmount;
    if (moveRight) _x += moveAmount;

	if (_x < 0) _x = 0;
	if (_x > (_mapColCount - _width)) _x = (_mapColCount - _width);

	if (_y < 0) _y = 0;
	if (_y > (_mapRowCount - _height)) _y = (_mapRowCount - _height);
}
