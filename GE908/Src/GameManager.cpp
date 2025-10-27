//
// Created by W.D. on 18/10/25
//

#include <string>
#include "GameManager.h"
#include "Foundation/GELog.h"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

using namespace GamesEngineeringBase;

GameManager::GameManager() : _window(), _font(), _mapManager(), _player(), _camera(WINDOW_WIDTH, WINDOW_HEIGHT), _isRunning(false) {}

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

	// load characters
	_player.load("Src/Assets/Textures/player.png");

	// update position
	const GESaveData* mapData = _mapManager.getSaveData();
	if (mapData) {
		int mapWorldWidth = mapData->getMapColCount() * mapData->getTileWidth();
		int mapWorldHeight = mapData->getMapRowCount() * mapData->getTileHeight();

		// set player in the center of map
		int playerStartX = static_cast<int>((mapWorldWidth / 2.0f) - (_player.getWidth() / 2.0f));
		int playerStartY = static_cast<int>((mapWorldHeight / 2.0f) - (_player.getHeight() / 2.0f));
		_player.setPosition(playerStartX, playerStartY);
		_player.setMapBounds(mapWorldWidth, mapWorldHeight);

		// set camera border
		_camera.setMapBounds(mapWorldWidth, mapWorldHeight);
	}
	else {
		GELog::shared().warning("Map data not loaded, player starts at (0,0)");
	}
}

void GameManager::run() {
	_isRunning = true;
	loadComponent();

	while (_isRunning) {
		update(0.016f);
		render();
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
}

void GameManager::render() {
	_window.drawMap(_mapManager, _camera);
	_window.drawPlayer(_player, _camera);
	_window.present();
}

void GameManager::stop() {
	_isRunning = false;

}