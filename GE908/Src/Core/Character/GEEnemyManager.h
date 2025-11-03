#pragma once
#include "GEEnemy.h"
#include "GEPlayer.h"

#define MAX_ENEMIES 200

class GEEnemyManager {

private:
    GEEnemy* _enemies[MAX_ENEMIES];
    unsigned int _enemyCount = 0;
    float _spawnTimer = 0.0f;
    float _spawnInterval = 10.0f;
    GESaveData* _saveData = nullptr;

    void spawnEnemyOutsideCamera(GEPlayer* player);

public:

    GEEnemyManager();
    ~GEEnemyManager();

    int getEnemyCount() const { return _enemyCount; };
    GEEnemy* getEnemyAt(int index) const { return _enemies[index]; }

    void load(GESaveData* saveData);
    void update(float deltaTime, GEPlayer* player);
    void draw(GEWindow& window, const GECamera& camera);

    bool isWaterTile(int tileID) { return tileID >= 14 && tileID <= 22; }
};
