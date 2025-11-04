#pragma once
#include "BaseCharacter.h"
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GESaveData.h"

class GEMapsManager;

class GEEnemyManager;

class GEPlayer : public BaseCharacter {
private:
	GESaveData* _saveData = nullptr;

	bool collidesWithWater(int newX, int newY, const GEMapsManager& mapsManager) const;

	bool collidesWithEnemies(int newX, int newY, const GEEnemyManager& enemyManager) const;

public:
	GEPlayer();
	~GEPlayer();

	void loadData(GESaveData* saveData);
	
	void update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight, const GEMapsManager& mapsManager, const GEEnemyManager& enemyManager);
};
