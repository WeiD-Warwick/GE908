#pragma once
#include "Foundation/GEFont.h"
#include "Foundation/GEFrameTimer.h"
#include "Foundation/GECamera.h"
#include "Foundation/GESaveData.h"
#include "Core/Maps/GEMapsManager.h"
#include "Core/Actors/GEPlayer.h"
#include "Core/Actors/GEEnemyManager.h"
#include "Core/Actors/GEProjectileManager.h"
#include "Core/Actors/GEPowerUp.h"

using namespace GamesEngineeringBase;

class GameManager {

private:
	Window _window;
	GEFont _font;
	GEPlayer _player;
	GECamera _camera;
	GEMapsManager _mapsManager;
	GEEnemyManager _enemyManager;
	GEProjectileManager _projectileManager;
	GEPowerUpManager _powerUpManager;
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

	void drawText();

	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
};

