#include <string>
#include "GameManager.h"
#define WINDOW_WIDTH 854
#define WINDOW_HEIGHT 480

const unsigned char fpsColor[3] = { 255, 0, 0 };

using namespace GamesEngineeringBase;

GameManager::GameManager() : _isRunning(false) {
	_window.create(WINDOW_WIDTH, WINDOW_HEIGHT, "WM908", false);
}

GameManager::~GameManager() {
	_font.release();
}

// load Game Resource, like maps, player, enemies
void GameManager::loadComponent() {
	_font.load();

	_mapsManager.load("Src/Assets/MapTiles/", "Src/SaveGames/tiles.txt");

	_saveData = _mapsManager.getSaveData();

	if (_saveData) {
		_player.bindWorldContext(&_mapsManager, &_enemyManager, &_projectileManager);

		int mapWorldWidth = _saveData->getMapTotalWidth();
		int mapWorldHeight = _saveData->getMapTotalHeight();
		_camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);
		_enemyManager.load(_saveData);
		_powerUpManager.load(_saveData);
	}
}


void GameManager::update(float deltaTime) {
	_window.checkInput();

	_player.update(deltaTime, _window);

	_camera.followPlayer(_player.getOriginX(), _player.getOriginY(), _player.getWidth(), _player.getHeight());

	_saveData->setCameraOffset(_camera.getX(), _camera.getY());

	_enemyManager.update(deltaTime, &_player, _projectileManager);
	_projectileManager.update(deltaTime, _enemyManager, _player);
	_powerUpManager.update(deltaTime, _player, _enemyManager);
}

void GameManager::render() {
	_window.clear();
	_mapsManager.draw(_window, _camera);
	_player.draw(_window, _camera);
	_enemyManager.draw(_window, _camera);
	_projectileManager.draw(_window, _camera);
	_powerUpManager.draw(_window, _camera);

	drawText();
	_window.present();
}

void GameManager::run() {
	_isRunning = true;
	loadComponent();

	while (_isRunning) {
		GEFrameTimer::shared().beginFrame();
		float dt = GEFrameTimer::shared().getDeltaTime();

		update(dt);
		render();

		GEFrameTimer::shared().endFrame();
	}
}


void GameManager::stop() {
	_isRunning = false;

}

void GameManager::drawText() {
	int killTextY = 20;
	_font.draw("Normal: " + std::to_string(_enemyManager.getKillCount(GEEnemyType::Normal)), 20, killTextY, fpsColor, 1, _window);
	killTextY += 20;
	_font.draw("Fast: " + std::to_string(_enemyManager.getKillCount(GEEnemyType::Fast)), 20, killTextY, fpsColor, 1, _window);
	killTextY += 20;
	_font.draw("Heavy: " + std::to_string(_enemyManager.getKillCount(GEEnemyType::Heavy)), 20, killTextY, fpsColor, 1, _window);
	killTextY += 20;
	_font.draw("Static: " + std::to_string(_enemyManager.getKillCount(GEEnemyType::StaticShooter)), 20, killTextY, fpsColor, 1, _window);

	_font.draw("FPS:" + std::to_string(static_cast<int>(GEFrameTimer::shared().getFPS())), 20, 400, fpsColor, 1, _window);
	_font.draw("HP:" + std::to_string(_player.getHP()), 100, 400, fpsColor, 1, _window);
	_font.draw("Skill: " + std::to_string(_player.getAOECooldownTime()), 400, 400, fpsColor, 1, _window);
}