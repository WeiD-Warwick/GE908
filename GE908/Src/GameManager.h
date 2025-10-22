//
// Created by W.D. on 18/10/25
//

#pragma once
#include "Foundation/GEWindow.h"
#include "Foundation/GEFont.h"

using namespace GamesEngineeringBase;

class GameManager {

private:
	// window
	GEWindow _window;
	GEFont _font;
	bool _isRunning;

public:
	GameManager();
	~GameManager();

	// Start Game Loops
	void run();

	// load Game Resource, like maps, player, enemies, fonts
	void loadComponent();

	// update
	void update(float deltaTime);

	// render
	void render();

	// End Game Loops
	void stop();
};

