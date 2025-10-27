#include "GEPlayer.h"
#include "../../../Foundation/GELog.h"
#include "../../../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

GEPlayer::GEPlayer() {}

GEPlayer::~GEPlayer() {}

void GEPlayer::load(const std::string& characterImagePath) {
	_characterImage = new Image();

	if (!_characterImage->load(characterImagePath)) {
		delete _characterImage;
		_characterImage = nullptr;
		GELog::shared().error("Load player character image: " + characterImagePath + "Failed");
	} else {
		_width = _characterImage->width;
		_height = _characterImage->height;
		GELog::shared().info("Load player character image: " + characterImagePath + "Success");
	}
}

void GEPlayer::update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight) {
    float moveDelta = _speed * deltaTime;
    unsigned int moveAmount = static_cast<unsigned int>(max(moveDelta, 1.0f)); 

	if (moveUp) {
		_y -= moveAmount;
	}
	if (moveDown) {
		_y += moveAmount;
	}
	if (moveLeft) {
		_x -= moveAmount;
	}
	if (moveRight) {
		_x += moveAmount;
	}
}
