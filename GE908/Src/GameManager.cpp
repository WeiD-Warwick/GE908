#include <string>
#include "GameManager.h"
#include "Foundation/GEDebug.h"
#define WINDOW_WIDTH 854
#define WINDOW_HEIGHT 480

const GEColor fpsColor = GEColor(255, 0, 0);

using namespace GamesEngineeringBase;

GameManager::GameManager(Window& window, MapService& mapSvc, PlayerService& player, EnemyService& enemySvc, ProjectileService& projectileSvc, PowerUpService& powerUpSvc) 
	: _window(window),
	_mapService(mapSvc),
	_player(player),
	_enemyService(enemySvc),
	_projectileService(projectileSvc),
	_powerUpService(powerUpSvc),
	_font(),
	_context(&_window, &_collisionSystem),
	_isRunning(false) {}

GameManager::~GameManager() = default;

void GameManager::loadComponent() {
	_font.load();
	_mapService.load("Src/Assets/MapTiles/", "Src/SaveGames/tiles.txt");
	_saveData = _mapService.getSaveData();

	if (_saveData) {
		_player.bindWorldContext(&_mapService, &_enemyService, &_projectileService);

		int mapWorldWidth = _saveData->getMapTotalWidth();
		int mapWorldHeight = _saveData->getMapTotalHeight();
		_camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);
		_enemyService.load(_saveData);
		_powerUpService.load(_saveData);
	}
}

void GameManager::update(float deltaTime) {
	_window.checkInput();
	GEDebug::shared().updateFromInput(_window);
	_player.update(deltaTime, _window);
	_camera.followPlayer(_player.collisionBody().getOriginX(), _player.collisionBody().getOriginY(), _player.collisionBody().getWidth(), _player.collisionBody().getHeight());
	_saveData->setCameraOffset(_camera.getX(), _camera.getY());
	_enemyService.update(deltaTime, &_player, _projectileService);
	_projectileService.update(deltaTime, _enemyService, _player);
	_powerUpService.update(deltaTime, _player);
}

void GameManager::render() {
	_window.clear();
	_mapService.draw(_window, _camera);
	_player.draw(_window, _camera);
	_enemyService.draw(_window, _camera);
	_projectileService.draw(_window, _camera);
	_powerUpService.draw(_window, _camera);

	drawText();
	_window.present();
}

void GameManager::run() {
	_isRunning = true;
	loadComponent();

	while (_isRunning) {
		GEFrameTimer::shared().beginFrame();
		float deltaTime = GEFrameTimer::shared().getDeltaTime();

		update(deltaTime);
		render();

		GEFrameTimer::shared().endFrame();
	}
}


void GameManager::stop() {
	_isRunning = false;

}

void GameManager::drawText() {
	int killTextY = 20;
	_font.draw("Normal: " + std::to_string(_enemyService.getKillCount(GEEnemyType::Normal)), GEPoint(20, killTextY), fpsColor, _window);
	killTextY += 20;
	_font.draw("Fast: " + std::to_string(_enemyService.getKillCount(GEEnemyType::Fast)), GEPoint(20, killTextY), fpsColor, _window);
	killTextY += 20;
	_font.draw("Heavy: " + std::to_string(_enemyService.getKillCount(GEEnemyType::Heavy)), GEPoint(20, killTextY), fpsColor, _window);
	killTextY += 20;
	_font.draw("Static: " + std::to_string(_enemyService.getKillCount(GEEnemyType::StaticShooter)), GEPoint(20, killTextY), fpsColor, _window);
	
	_font.draw("FPS:" + std::to_string(static_cast<int>(GEFrameTimer::shared().getFPS())), GEPoint(20, 400), fpsColor, _window);
	_font.draw("HP:" + std::to_string(_player.getHP()), GEPoint(200, 400), fpsColor, _window);
	_font.draw("Skill: " + std::to_string(_player.getAOECooldownTime()), GEPoint(400, 400), fpsColor, _window);
}