#pragma once

enum class GEEventType {
	Character_DEATH				= 0,
	PLAYER_HURT,
	POWERUP_PICKED,
	PROJECTILE_HIT,

	COLLISION				= 20,
	ENEMY_COLLIDE_PROJECTILE,
	PLAYER_COLLIDE_WATER,
	PLAYER_COLLIDE_FIRE,
	PLAYER_COLLIDE_ENEMY,
	PLAYER_COLLIDE_ENVIRONMENT,

	ENEMY_ATTACK,
	
};

enum class GEPowerUpType {
	None					= 0,
	AttackSpeedBoost,
	AdditionalAoeTarget,
};

enum class ProjectileOwner {
	FromPlayer				= 0,
	FromEnemy,
};

enum class GEEnemyType {
	Normal					= 0,
	Fast,
	Heavy,
	StaticShooter,
};

enum class GEColliderType {
	None					= 0,
	Player,
	Enemy,
	Projectile,
	PowerUp,
	Tile_Water,
	Tile_Fire,
};

enum class GEColliderShape {
	Circle					= 0,
	AABB,
};