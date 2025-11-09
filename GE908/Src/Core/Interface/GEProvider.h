#pragma once
#include <string>
#include "../SaveLoad/GESaveData.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEModel.h"

class GEContext;

class GETile;

class EnemyProvider {
public:
	virtual void load(GESaveData* saveData) = 0;
	virtual void update(float deltaTime, GEContext& ctx) = 0;
	virtual int getEnemyCount() const = 0;
	virtual GECollisible* getEnemyAt(int index) const = 0;
	virtual void registerEnemyKill(GEEnemyType type) = 0;
	virtual int getKillCount(GEEnemyType type) const = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
};

class ProjectileProvider {
public:
	virtual void update(float deltaTime, GEContext& ctx) = 0;
	virtual void addProjectile(ProjectileOwner from, float startPointX, float startPointY, float dirX, float dirY, float speed, int damage) = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
};

class PowerUpProvider {
public:
	virtual void load(GESaveData* saveData) = 0;
	virtual void update(float deltaTime, GEContext& ctx) = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
	virtual void onEnemyDefeated(const GEPoint& position) = 0;
};

class MapProvider {
public:
	virtual GESaveData* getSaveData() const = 0;
	virtual GETile* getTile(int tileID) const = 0;
	virtual void draw(Window& window, const GECamera& camera) const = 0;
	virtual void load(const std::string& tilesFolderPath, const std::string& saveDatafilePath) = 0;
};


class PlayerProvider {
public:
	virtual void takeDamage(int value) = 0;
	virtual void update(float deltaTime, Window& window) = 0;
	virtual GECollisible& collisionBody() = 0;
	virtual void applyPowerUp(GEPowerUpType type) = 0;
	virtual int getHP() const = 0;
	virtual float getAOECooldownTime() const = 0;
	virtual void draw(Window& window, const GECamera& camera) const = 0;
	virtual void bind(GEContext& ctx) = 0;
};

