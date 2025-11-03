#pragma once
#include <string>
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Core/Camera/GECamera.h"
#include "GELog.h"

class GEWindow : public GamesEngineeringBase::Window {
 
public:

    GEWindow();
    ~GEWindow();

    void load(int width, int height, const std::string& title, bool isFullstreen);

};

