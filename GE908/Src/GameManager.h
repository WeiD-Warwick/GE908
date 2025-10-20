//
// Created by W.D. on 18/10/25
//

#pragma once
#include "GEKit/GEWindow.h"

using namespace GamesEngineeringBase;

class GameManager {

private:
	// window
	GEWindow _window;
	bool _isRunning;
	void _loadWindow(int width, int height, const std::string title, bool fullScreen);

public:
	GameManager();
	~GameManager();

	// Start Game Loops
	void run();

	// load Game Resource, like maps, player, enemies
	void loadView();

	// update
	void update(float deltaTime);

	// render
	void render();

	// End Game Loops
	void stop();
};

