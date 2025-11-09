#pragma once

enum class GEPowerUpType {
	None					= 0,
	AttackSpeedBoost		= 1,
	AdditionalAoeTarget		= 2,
	HealPlayer				= 3,
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

enum class GECollisionType {
	None,
	Player,
	Enemy,
	Water,
	Fire,
	Projectile,
	PowerUp,
};

enum class GEColliderShape {
	Circle					= 0,
	AABB					= 1,
};