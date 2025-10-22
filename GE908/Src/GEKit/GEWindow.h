//
// Created by W.D. on 20/10/25
//

#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "GEColor.h"
#include <string>

class GEWindow : public GamesEngineeringBase::Window {
private:
	// set the backgroundColor
	void drawBGColor();
	void loadFont();
public:

	GEColor backgroundColor = GEColor(0, 0, 0);

	GEWindow();
	~GEWindow();

	// When App Start, play launch app animation
	void playLaunchAppAnimation(float duration);

	// Draw Text
	void drawText(const std::string& text, int startX, int startY, const GEColor& color, int scale);

	// render UI in game loops
	void render();
};

