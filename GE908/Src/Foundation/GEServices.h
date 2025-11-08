#pragma once
#include <string>
#include "GECollisible.h"
#include "GEModel.h"
#include "GESaveData.h"

class ProjectileService;
class MapService;
class PowerUpService;
class PlayerService;

class EnemyService {
public:
	virtual void load(GESaveData* saveData) = 0;
	virtual void update(float deltaTime, PlayerService* player, ProjectileService& projectileManager) = 0;
	virtual int getEnemyCount() const = 0;
	virtual GECollisible* getEnemyAt(int index) const = 0;
	virtual void registerEnemyKill(GEEnemyType type) = 0;
	virtual int getKillCount(GEEnemyType type) const = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
};

class ProjectileService {
public:
	virtual void update(float deltaTime, EnemyService& enemyManager, PlayerService& player) = 0;
	virtual void addProjectile(ProjectileOwner from, float startPointX, float startPointY, float dirX, float dirY, float speed, int damage) = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
};

class PowerUpService {
public:
	virtual void load(GESaveData* saveData) = 0;
	virtual void update(float deltaTime, PlayerService& player) = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
};

class MapService {
public:
	virtual GESaveData* getSaveData() const = 0;
	virtual GETile* getTile(int tileID) const = 0;
	virtual void draw(Window& window, const GECamera& camera) const = 0;
	virtual void load(const std::string& tilesFolderPath, const std::string& saveDatafilePath) = 0;
};


class PlayerService {
public:
	virtual void takeDamage(int value) = 0;
	virtual void update(float deltaTime, Window& window) = 0;
	virtual GECollisible& collisionBody() = 0;
	virtual void applyPowerUp(GEPowerUpType type) = 0;
	virtual int getHP() const = 0;
	virtual float getAOECooldownTime() const = 0;
	virtual void draw(Window& window, const GECamera& camera) const = 0;
	virtual void bindWorldContext(const MapService* maps, EnemyService* enemies, ProjectileService* projectiles) = 0;
};

