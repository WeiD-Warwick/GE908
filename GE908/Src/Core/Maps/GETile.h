#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GECollisible.h"

using namespace GamesEngineeringBase;

class GETile : public GECollisible {

public:
	GETile(const std::string& filename, GECollisionType collisionType);

	~GETile();

};

