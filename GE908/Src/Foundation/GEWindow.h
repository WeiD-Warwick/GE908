//
// Created by W.D. on 20/10/25
//

#pragma once
#include <string>
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Core/Maps/GEMapsManager.h"
#include "GEFont.h"
#include "GELog.h"

class GEWindow : public GamesEngineeringBase::Window {
 
public:

    GEWindow();
    ~GEWindow();

    void load(int width, int height, const std::string& title, bool isFullstreen);

	// Draw Text
    void drawText(const std::string& text, int startX, int startY, const unsigned char* textColor, int scale);

    void drawImage(const Image* image, int x, int y);

    void drawMap(GEMapsManager& mapManager, int offsetX, int offsetY);
};

