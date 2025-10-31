#pragma once
#include "GELog.h"
#include "../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class GEFont {
private:
    static Image* fontChar[95]; // 32~126 ASCII, offSet 32

    static bool isLoaded;

public:

	// load font characters from folder
    static bool load(); 

	// get character image by char
    static Image* getChar(char c);

	// release font resources
    static void release();
};
