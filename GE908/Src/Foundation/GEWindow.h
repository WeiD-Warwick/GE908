#pragma once
#include <string>
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Core/Maps/GEMapsManager.h"
#include "../Core/Camera/GECamera.h"
#include "GEFont.h"
#include "GELog.h"

class GEWindow : public GamesEngineeringBase::Window {
 
public:

    GEWindow();
    ~GEWindow();

    void load(int width, int height, const std::string& title, bool isFullstreen);

    void drawText(const std::string& text, int startX, int startY, const unsigned char* textColor, int scale);

    void drawMap(GEMapsManager& mapManager, const GECamera& camera);
};

