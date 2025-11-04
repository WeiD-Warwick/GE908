#pragma once
#include "Foundation/GEWindow.h"
#include "Foundation/GEFont.h"
#include "Foundation/GEFPSCounter.cpp"
#include "Core/Maps/GEMapsManager.h"
#include "Core/Character/GEPlayer.h"
#include "Core/Camera/GECamera.h"
#include "Core/Character/GEEnemyManager.h"

using namespace GamesEngineeringBase;

class GameManager {

private:
	GEWindow _window;
	GEFont _font;
	GEMapsManager _mapsManager;
	GEPlayer _player;
	GECamera _camera;
	GEFpsCounter _fpsCounter;
	GEEnemyManager _enemyManager;
	GESaveData* _saveData = nullptr;

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

	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
};

