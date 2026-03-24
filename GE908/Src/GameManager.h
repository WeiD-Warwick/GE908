#pragma once
#include "Foundation/GEFont.h"
#include "Foundation/GEFrameTimer.h"
#include "Foundation/GECamera.h"
#include "Core/SaveLoad/GESaveData.h"
#include "Core/Entity/GEPlayer.h"
#include "Core/Interface/GEProvider.h"
#include "Foundation/GEContext.h"
#include <memory>

using namespace GamesEngineeringBase;

class GameManager {
private:
    Window& _window;
    GEFont _font;
    GECamera _camera;
    std::unique_ptr<GESaveData> _saveData;
    MapProvider& _mapProvider;
    PlayerProvider& _player;
    EnemyProvider& _enemyProvider;
    ProjectileProvider& _projectileProvider;
    PowerUpProvider& _powerUpProvider;
    GEContext _ctx;
    bool _isRunning = false;

    GEGameLifeCircle _gameState = GEGameLifeCircle::Menu;
    GEDataLoadMode _loadMode = GEDataLoadMode::NewFix;

    float _levelTimeRemaining = 120.0f;

    bool _componentHasLoaded = false;

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
        _gameState = GEGameLifeCircle::Menu;
    }

    ~GameManager() = default;

	// Start Game Loops
	void run();

	// load Game Resource, like , player, enemies, fonts
	void loadComponent(const GEDataLoadMode loadMode);

	// update
	void update(float deltaTime);

	// render
	void render();

	// End Game Loops
	void stop();

    void updateMenu();

    void renderMenu();

    void updateEnding();

    void renderEnding();

	void drawText();

	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
};

