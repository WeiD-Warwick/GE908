//
// Created by W.D. on 20/10/25
//

#pragma once
#include <string>
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "GEModel.h"
#include "GEFont.h"
#include "GELog.h"

class GEWindow : public GamesEngineeringBase::Window {

private:
    void drawBGColor();

public:
	GEColor backgroundColor = GEColor(0, 0, 0);

    GEWindow();
    ~GEWindow();

    void loadWindow();

	// When App Start, play launch app animation
    void _playLaunchAppAnimation(float duration);

	// Draw Text
    void drawText(const std::string& text, int startX, int startY, const GEColor& color, int scale);

	// render UI in game loops
    void render();
};

