#pragma once
#include "Foundation/GEFont.h"
#include "Foundation/GEFrameTimer.h"
#include "Foundation/GECamera.h"
#include "Foundation/GESaveData.h"
#include "Core//GEMapsManager.h"
#include "Core/GEPlayer.h"
#include "Foundation/GEServices.h"
#include "Foundation/GEContext.h"
#include "Engine/CollisionSystem.h"


using namespace GamesEngineeringBase;

class GameManager {

private:
	Window& _window;
	GEFont _font;
	GECamera _camera;
	MapService& _mapService;
	PlayerService& _player;
	EnemyService& _enemyService;
	ProjectileService& _projectileService;
	PowerUpService& _powerUpService;
	GESaveData* _saveData = nullptr;

	//////////////////////////////
	GEContext _context;
	CollisionSystem _collisionSystem;


	//////////////////////////////

	bool _isRunning = false;

public:
	GameManager(Window& window, MapService& mapSvc, PlayerService& player, EnemyService& enemySvc, ProjectileService& projectileSvc, PowerUpService& powerUpSvc);
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

