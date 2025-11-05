#pragma once
#include "Foundation/GEWindow.h"
#include "Foundation/GEFont.h"
#include "Foundation/GEFPSCounter.cpp"
#include "Core/Maps/GEMapsManager.h"
#include "Core/Character/GEPlayer.h"
#include "Core/Camera/GECamera.h"
#include "Core/Character/GEEnemyManager.h"
#include "Core/Items/GEProjectileManager.h"
#include "Core/Items/GEBuffManager.h"

using namespace GamesEngineeringBase;

class GameManager {

private:
	GEWindow _window;
	GEFont _font;
	GEMapsManager _mapsManager;
	GEPlayer _player;
	GECamera _camera;
	GEFrameTimer _frameTimer;
	GEEnemyManager _enemyManager;
	GEProjectileManager _projectileManager;
	GEBuffManager _buffManager;
	GESaveData* _saveData = nullptr;

	bool _isRunning;
	bool _pressSkill = false;

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

