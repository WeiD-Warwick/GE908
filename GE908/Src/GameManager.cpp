#include <string>
#include "GameManager.h"
#include "Foundation/GELog.h"
#define WINDOW_WIDTH 854
#define WINDOW_HEIGHT 480

using namespace GamesEngineeringBase;

GameManager::GameManager() : _window(), _font(), _mapManager(), _player(), _camera(), _isRunning(false) {}

GameManager::~GameManager() {
	_font.release();
}

// load Game Resource, like maps, player, enemies
void GameManager::loadComponent() {

	_window.load(WINDOW_WIDTH, WINDOW_HEIGHT, "WM908", false);

	_font.load();

	_mapManager.load("Src/Assets/MapTiles/", "Src/SaveGames/tiles.txt");

	_saveData = _mapManager.getSaveData();
	if (_saveData) {
		_player.load("Src/Assets/Textures/player.png");
		_player.loadData(_saveData);

		int mapWorldWidth = _saveData->getMapTotalWidth();
		int mapWorldHeight = _saveData->getMapTotalHeight();
		_camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);
		_enemyManager.load(_saveData);
	}
	else {
		GELog::shared().warning("Map data not loaded, player starts at (0,0)");
	}
}


void GameManager::update(float deltaTime) {
	_window.checkInput();

	bool moveUp = _window.keyPressed('W');
	bool moveDown = _window.keyPressed('S');
	bool moveLeft = _window.keyPressed('A');
	bool moveRight = _window.keyPressed('D');
	bool pause = _window.keyPressed(VK_ESCAPE);

	if (pause) stop();

	_player.update(deltaTime, moveUp, moveDown, moveLeft, moveRight, _enemyManager);

	for (int i = 0; i < _enemyManager.getEnemyCount(); ++i) {
		GEEnemy* enemy = _enemyManager.getEnemyAt(i);
		if (enemy && enemy->collide(_player)) {
			GELog::shared().warning("Player hit by enemy!");
			break;
		}
	}

	_camera.followPlayer(_player.getX(), _player.getY(), _player.getWidth(), _player.getHeight());

	_saveData->setCameraOffset(_camera.getX(), _camera.getY());

	_enemyManager.update(deltaTime, &_player);

	_fpsCounter.frameRendered();
}

void GameManager::render() {
	_window.clear();
	_mapManager.draw(_window, _camera);
	_player.draw(_window, _camera);
	_enemyManager.draw(_window, _camera);

	const unsigned char fpsColor[3] = { 255, 0, 0 };

	_font.draw("FPS:" + std::to_string(static_cast<int>(_fpsCounter.getFps())), 200, 400, fpsColor, 1, _window);

	_window.present();
}

void GameManager::run() {
	_isRunning = true;
	loadComponent();

	while (_isRunning) {
		update(0.016f);
		render();
	}
}


void GameManager::stop() {
	_isRunning = false;

}