//
// Created by W.D. on 20/10/25
//

#pragma once
#include <string>
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Core/Maps/GEMapsManager.h"
#include "../Core/Camera/GECamera.h"
#include "../Core/Character/Player/GEPlayer.h"
#include "GEFont.h"
#include "GELog.h"
#include "GEButton.h"

class GEWindow : public GamesEngineeringBase::Window {
 
public:

    GEWindow();
    ~GEWindow();

    void load(int width, int height, const std::string& title, bool isFullstreen);

    void drawText(const std::string& text, int startX, int startY, const unsigned char* textColor, int scale);

    void drawMap(GEMapsManager& mapManager, const GECamera& camera);

    void drawPlayer(const GEPlayer& player, const GECamera& camera);

    void drawUI(const GEButton* buttons);
};

