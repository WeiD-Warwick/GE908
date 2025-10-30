//
// Created by W.D. on 18/10/25
//

#pragma once
#include "Foundation/GEWindow.h"
#include "Foundation/GEFont.h"
#include "Core/Maps/GEMapsManager.h"
#include "Core/Character/GEPlayer.h"
#include "Core/Camera/GECamera.h"
#include "Foundation/GEFPSCounter.cpp"

using namespace GamesEngineeringBase;

class GameManager {

private:
	GEWindow _window;
	GEFont _font;
	GEMapsManager _mapManager;
	GEPlayer _player;
	GECamera _camera;
	GEFpsCounter _fpsCounter;

	bool _isRunning;

public:
	GameManager();
	~GameManager();

	// Start Game Loops
	void run();

	// load Game Resource, like maps, player, enemies, fonts
	void loadComponent();

	// update
	void update(float deltaTime);

	// render
	void render();

	// End Game Loops
	void stop();
};

