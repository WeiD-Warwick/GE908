//
// Created by W.D. on 18/10/25
//

#include <string>
#include "GameManager.h"
#include "Foundation/GELog.h"

using namespace GamesEngineeringBase;

GameManager::GameManager() : _window(), _font(), _mapManager(), _isRunning(false) {}

GameManager::~GameManager() {
	_font.release();
}

// load Game Resource, like maps, player, enemies
void GameManager::loadComponent() {

	// load window
	_window.loadWindow();

	// load font
	_font.loadFont();

	// load maps
	_mapManager.loadTileResources("Src/Assets/Tiles/");
	_mapManager.loadSaveData("Src/SaveGames/tiles.txt");

	// load sounds
	
	// load characters
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
}

void GameManager::render() {
	_window.drawMap(_mapManager, 0, 0);
	_window.present();
}

void GameManager::stop() {
	_isRunning = false;

}