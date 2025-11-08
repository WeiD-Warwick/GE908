#pragma once

enum class GEPowerUpType {
	None					= 0,
	AttackSpeedBoost		= 1,
	AdditionalAoeTarget		= 2,
};

enum class ProjectileOwner {
	FromPlayer				= 0,
	FromEnemy				= 1,
};

enum class GEEnemyType {
	Normal					= 0,
	Fast					= 1,
	Heavy					= 2,
	StaticShooter			= 3,
};

enum class GEColliderType {
	None					= 0,
	Player					= 1,
	Enemy					= 2,
	Water					= 3,
	Fire					= 4,
	Projectile				= 5,
	PowerUp					= 6,
};

enum class GEColliderShape {
	Circle					= 0,
	AABB					= 1,
};

class GETile : public GECollisible {

public:
	GETile(const std::string& filename, GECollisionType collisionType)
		: GECollisible(filename, collisionType) {
	}

	~GETile() = default;
};