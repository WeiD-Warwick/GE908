#pragma once
#include "Foundation/GEFont.h"
#include "Foundation/GEFrameTimer.h"
#include "Foundation/GECamera.h"
#include "Core/SaveLoad/GESaveData.h"
#include "Core/Entity/GEPlayer.h"
#include "Core/Interface/GEProvider.h"
#include "Foundation/GEContext.h"

using namespace GamesEngineeringBase;

class GameManager {
private:
    Window& _window;
    GEFont _font;
    GECamera _camera;
    GESaveData* _saveData = nullptr;
    MapProvider& _mapProvider;
    PlayerProvider& _player;
    EnemyProvider& _enemyProvider;
    ProjectileProvider& _projectileProvider;
    PowerUpProvider& _powerUpProvider;
    GEContext _ctx;
    bool _isRunning = false;

public:
    GameManager(Window& window,
        MapProvider& mapProvider,
        PlayerProvider& playerProvider,
        EnemyProvider& enemyProvider,
        ProjectileProvider& projectileProvider,
        PowerUpProvider& powerUpProvider)
        : _window(window),
        _mapProvider(mapProvider),
        _player(playerProvider),
        _enemyProvider(enemyProvider),
        _projectileProvider(projectileProvider),
        _powerUpProvider(powerUpProvider),
        _ctx(window, enemyProvider, mapProvider, projectileProvider, powerUpProvider, playerProvider)
    {
    }

    ~GameManager() = default;

	// Start Game Loops
	void run();

	// load Game Resource, like , player, enemies, fonts
	void loadComponent(const std::string& dataPath);

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

