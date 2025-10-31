//
// Created by W.D. on 18/10/25
//

#include <string>
#include "GameManager.h"
#include "Foundation/GELog.h"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

using namespace GamesEngineeringBase;

GameManager::GameManager() : _window(), _font(), _mapManager(), _player(), _camera(), _isRunning(false) {}

GameManager::~GameManager() {
	_font.release();
}

// load Game Resource, like maps, player, enemies
void GameManager::loadComponent() {

	// load window
	_window.load(WINDOW_WIDTH, WINDOW_HEIGHT, "WM908", false);

	// load font
	_font.load();

	// load maps
	_mapManager.load("Src/Assets/MapTiles/", "Src/SaveGames/tiles.txt");

	// update position
	GESaveData* saveData = _mapManager.getSaveData();
	if (saveData) {
		int mapWorldWidth = saveData->getMapTotalWidth();
		int mapWorldHeight = saveData->getMapTotalHeight();

		// set player in the center of map
		int playerStartX = static_cast<int>((mapWorldWidth / 2.0f) - (_player.getWidth() / 2.0f));
		int playerStartY = static_cast<int>((mapWorldHeight / 2.0f) - (_player.getHeight() / 2.0f));
		
		_player.load("Src/Assets/Textures/player.png");
		_player.loadData(saveData);
		_camera.loadData(saveData);
		_enemyManager.load(saveData);
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

	_player.update(deltaTime, moveUp, moveDown, moveLeft, moveRight);

	_camera.followPlayer(_player.getX(), _player.getY(), _player.getWidth(), _player.getHeight());

	_enemyManager.update(deltaTime, &_player);

	for (int i = 0; i < _enemyManager.getEnemyCount(); ++i) {
		GEEnemy* e = _enemyManager.getEnemyAt(i);
		if (e && e->collide(_player)) {
			GELog::shared().warning("Player hit by enemy!");
		}
	}

	_fpsCounter.frameRendered();
}

void GameManager::render() {
	_window.clear();
	_window.drawMap(_mapManager, _camera);
	_player.draw(_window, _camera);
	_enemyManager.draw(_window, _camera);

	const unsigned char fpsColor[3] = { 255, 0, 0 };
	_window.drawText("FPS:" + std::to_string(static_cast<int>(_fpsCounter.getFps())), 1000, 700, fpsColor, 1);

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