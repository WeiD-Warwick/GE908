#pragma once
#include "GEEnemy.h"
#include "../Foundation/GESaveData.h"
#include "../Foundation/GEServices.h"

static constexpr auto MAX_ENEMIES = 1000;
static constexpr float DEFAULT_SPAWN_INTERVAL = 6.0f;
static constexpr float MIN_SPAWN_INTERVAL = 2.5f;
static constexpr float SPAWN_INTERVAL_STEP = 0.25f;
static constexpr float SPAWN_DIFFICULTY_STEP_TIME = 25.0f;

static constexpr int BASE_ACTIVE_ENEMY_CAP = 12;
static constexpr int ACTIVE_ENEMY_CAP_INCREMENT = 4;
static constexpr float ACTIVE_ENEMY_CAP_STEP_TIME = 45.0f;

static constexpr int ENEMY_TYPE_COUNT = 4;

class GEEnemyManager : public EnemyService {

private:
    GEEnemy* _enemies[MAX_ENEMIES];
    int _enemyCount = 0;
    float _spawnTimer = 0.0f;
    float _spawnInterval = DEFAULT_SPAWN_INTERVAL;
    float _difficultyTimer = 0.0f;
    float _elapsedTime = 0.0f;

    int _killCounts[ENEMY_TYPE_COUNT] = { 0 };

    GESaveData* _saveData = nullptr;

    void spawnEnemyOutsideCamera(PlayerService* player);

public:

    GEEnemyManager();
    ~GEEnemyManager();

    int getEnemyCount() const { return _enemyCount;}
    GECollisible *getEnemyAt(int index) const { return _enemies[index]; }

    void load(GESaveData* saveData);
    void update(float deltaTime, PlayerService* player, ProjectileService& projectileManager);
    void draw(Window& window, const GECamera& camera);

    void registerEnemyKill(GEEnemyType type);
    void resetKillCounts();
    int getKillCount(GEEnemyType type) const;

    bool isWaterTile(int tileID) { return tileID >= 14 && tileID <= 22;}
};
