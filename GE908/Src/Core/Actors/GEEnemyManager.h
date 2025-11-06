#pragma once
#include "GEEnemy.h"
#include "../../Foundation/GESaveData.h"

static constexpr auto MAX_ENEMIES = 1000;
static constexpr float DEFAULT_SPAWN_INTERVAL = 6.0f;
static constexpr float MIN_SPAWN_INTERVAL = 2.5f;
static constexpr float SPAWN_INTERVAL_STEP = 0.25f;
static constexpr float SPAWN_DIFFICULTY_STEP_TIME = 25.0f;

static constexpr int BASE_ACTIVE_ENEMY_CAP = 12;
static constexpr int ACTIVE_ENEMY_CAP_INCREMENT = 4;
static constexpr float ACTIVE_ENEMY_CAP_STEP_TIME = 45.0f;

class GEPlayer;

class GEProjectileManager;

class GEEnemyManager {

private:
    GEEnemy* _enemies[MAX_ENEMIES];
    int _enemyCount = 0;
    float _spawnTimer = 0.0f;
    float _spawnInterval = DEFAULT_SPAWN_INTERVAL;
    float _difficultyTimer = 0.0f;
    float _elapsedTime = 0.0f;

    GESaveData* _saveData = nullptr;

    void spawnEnemyOutsideCamera(GEPlayer* player);

public:

    GEEnemyManager();
    ~GEEnemyManager();

    int getEnemyCount() const { return _enemyCount;}
    GEEnemy* getEnemyAt(int index) const { return _enemies[index];}

    void load(GESaveData* saveData);
    void update(float deltaTime, GEPlayer* player, GEProjectileManager& projectileManager);
    void draw(Window& window, const GECamera& camera);

    bool isWaterTile(int tileID) { return tileID >= 14 && tileID <= 22;}
};
