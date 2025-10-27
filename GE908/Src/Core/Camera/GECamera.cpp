//
// Created by W.D. on 22/10/25
//

#include "GECamera.h"
#include "../Character/Player/GEPlayer.h"

GECamera::GECamera(int screenW, int screenH)
	: _offsetX(0), _offsetY(0), _screenW(screenW), _screenH(screenH) {
}

GECamera::~GECamera() {}


// calculate left top world coordinates
// make the character in the center of the screen
void GECamera::followPlayer(GEPlayer& player) {
	_offsetX = player.getX() - (_screenW / 2) + (player.getWidth() / 2);
	_offsetY = player.getY() - (_screenH / 2) + (player.getHeight() / 2);
}
