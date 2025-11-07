#pragma once
#include "Foundation/GEFont.h"
#include "Foundation/GEFrameTimer.h"
#include "Foundation/GECamera.h"
#include "Foundation/GESaveData.h"
#include "Core//GEMapsManager.h"
#include "Core/GEPlayer.h"
#include "Core/GEEnemyManager.h"
#include "Core/GEProjectileManager.h"
#include "Core/GEPowerUp.h"

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

	// load Game Resource, like , player, enemies, fonts
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

