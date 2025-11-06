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
	}
}


void GameManager::update(float deltaTime) {
	_window.checkInput();

	_player.update(deltaTime, _window);

	_camera.followPlayer(_player.getOriginX(), _player.getOriginY(), _player.getWidth(), _player.getHeight());

	_saveData->setCameraOffset(_camera.getX(), _camera.getY());

	_enemyManager.update(deltaTime, &_player, _projectileManager);
	_projectileManager.update(deltaTime, _enemyManager, _player);
}

void GameManager::render() {
	_window.clear();
	_mapsManager.draw(_window, _camera);
	_player.draw(_window, _camera);
	_enemyManager.draw(_window, _camera);
	_projectileManager.draw(_window, _camera);

	_font.draw("FPS:" + std::to_string(static_cast<int>(GEFrameTimer::shared().getFPS())), 200, 400, fpsColor, 1, _window);
	_font.draw("HP:" + std::to_string(_player.getHP()), 400, 400, fpsColor, 1, _window);
	_font.draw("Skill: " + std::to_string(_player.getAOECooldownTime()), 600, 400, fpsColor, 1, _window);

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