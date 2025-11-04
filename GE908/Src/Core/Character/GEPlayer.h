#pragma once
#include "BaseCharacter.h"
#include "../../Foundation/GEWindow.h"
#include "../../Foundation/GESaveData.h"

class GEEnemyManager;

class GEPlayer : public BaseCharacter {
private:
	GESaveData* _saveData = nullptr;

	bool collidesWithWater(int newX, int newY) const;

	bool collidesWithEnemies(int newX, int newY, const GEEnemyManager& enemyManager) const;

public:
	GEPlayer();
	~GEPlayer();

	void loadData(GESaveData* saveData);
	
	void update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight, const GEEnemyManager& enemyManager);
};
