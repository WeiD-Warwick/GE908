#pragma once
#include "../../Foundation/GECollisible.h"
#include <string>

class GETile : public GECollisible {

public:
	GETile(const std::string& filename, GECollisionType collisionType)
		: GECollisible(filename, collisionType) {
	}

	~GETile() = default;
};