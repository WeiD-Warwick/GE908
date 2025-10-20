//
// Created by W.D. on 18/10/25
//

#include <string>
#include "GameManager.h"
#include "Foundation/GELog.h"

using namespace GamesEngineeringBase;

void GameManager::_loadWindow(int width, int height, const std::string title, bool fullScreen) {
	_window.create(width, height, title, fullScreen);
	GELog::shared().info("Init window.");
	_window.playLaunchAppAnimation(5.f);
}

GameManager::GameManager() : _window(), _isRunning(false) {}

GameManager::~GameManager() {}

void GameManager::run() {
	_isRunning = true;
	loadView();

	while (_isRunning) {
		update(0.016f);
		render();
	}
}

// load Game Resource, like maps, player, enemies
void GameManager::loadView() {
	_loadWindow(1280, 720, "WM908", false);
}

void GameManager::update(float deltaTime) {
	_window.checkInput();
}

void GameManager::render() {
	_window.render();
}

void GameManager::stop() {

}