#include "GEEnemyManager.h"
#include "../Actors/GEProjectileManager.h"
#include "GEPlayer.h"
#include <cstdlib>
#include <ctime>

namespace {
    constexpr int PLAYER_COLLISION_DAMAGE = 30;
    constexpr int ENEMY_COLLISION_DAMAGE = 30;
}

GEEnemyManager::GEEnemyManager() {
    for (int i = 0;i < MAX_ENEMIES;i++) _enemies[i] = nullptr;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

GEEnemyManager::~GEEnemyManager() {
    for (int i = 0;i < MAX_ENEMIES;i++) {
        if (_enemies[i]) {
            delete _enemies[i];
            _enemies[i] = nullptr;
        }
    }
}

void GEEnemyManager::load(GESaveData* saveData) {
    _saveData = saveData;
    _spawnInterval = DEFAULT_SPAWN_INTERVAL;
    _spawnTimer = 0.0f;
    _difficultyTimer = 0.0f;
}

void GEEnemyManager::spawnEnemyOutsideCamera(GEPlayer* player) {
    if (_enemyCount >= MAX_ENEMIES) return;

    float camOffsetX = _saveData->getCameraOffsetX();
    float camOffsetY = _saveData->getCameraOffsetY();
    int screenW = _saveData->getScreenWidth();
    int screenH = _saveData->getScreenHeight();
    int mapW = _saveData->getMapTotalWidth();
    int mapH = _saveData->getMapTotalHeight();

    // make the enemy appearance more natural
    const int safeDistance = _saveData->getTileWidth() * 2;
    int side = rand() % 4;
    float x = 0;
    float y = 0;

    switch (side) {
    case 0:
        // top
        x = camOffsetX + rand() % screenW;
        y = camOffsetY - safeDistance;
        break;
    case 1: 
        // bottom
        x = camOffsetX + rand() % screenW;
        y = camOffsetY + screenH + safeDistance;
        break;
    case 2: 
        // left
        x = camOffsetX - safeDistance;
        y = camOffsetY + rand() % screenH;
        break;
    case 3: 
        // right
        x = camOffsetX + screenW + safeDistance;
        y = camOffsetY + rand() % screenH;
        break;
    }

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > mapW - 1) x = mapW - 1;
    if (y > mapH - 1) y = mapH - 1;

    GEEnemyType type = static_cast<GEEnemyType>(rand() % 4);
    GEEnemy* enemy = new GEEnemy(type);

    enemy->setMapBounds(mapW, mapH);
    enemy->setCenter(x, y);

    _enemies[_enemyCount++] = enemy;
}

void GEEnemyManager::draw(Window& window, const GECamera& camera) {
    for (unsigned int i = 0;i < _enemyCount;i++) {
        GEEnemy* enemy = _enemies[i];
        if (!enemy || !enemy->isAlive()) continue;
        enemy->draw(window, camera);
    }
}

void GEEnemyManager::update(float deltaTime, GEPlayer* player, GEProjectileManager& projectileManager) {
    _spawnTimer += deltaTime;
    _difficultyTimer += deltaTime;
    _elapsedTime += deltaTime;

    const int capSteps = static_cast<int>(_elapsedTime / ACTIVE_ENEMY_CAP_STEP_TIME);
    const int activeEnemyCap = min(MAX_ENEMIES, BASE_ACTIVE_ENEMY_CAP + capSteps * ACTIVE_ENEMY_CAP_INCREMENT);

    if (_enemyCount >= activeEnemyCap) {
        _spawnTimer = min(_spawnTimer, _spawnInterval);
    }
    else {
        while (_spawnTimer >= _spawnInterval) {
            const int previousCount = _enemyCount;
            spawnEnemyOutsideCamera(player);
            _spawnTimer -= _spawnInterval;

            if (_enemyCount == previousCount || _enemyCount >= activeEnemyCap) {
                if (_enemyCount == previousCount) {
                    _spawnTimer = 0.0f;
                }
                break;
            }
        }
    }

    if (_difficultyTimer >= SPAWN_DIFFICULTY_STEP_TIME) {
        _difficultyTimer -= SPAWN_DIFFICULTY_STEP_TIME;
        _spawnInterval = max(_spawnInterval - SPAWN_INTERVAL_STEP, MIN_SPAWN_INTERVAL);
    }

    for (int i = 0;i < _enemyCount;i++) {
        GEEnemy* enemy = _enemies[i];
        if (!enemy || !enemy->isAlive()) continue;

        const float previousX = enemy->getCenterX();
        const float previousY = enemy->getCenterY();

        enemy->update(deltaTime, player->getCenterX(), player->getCenterY(), projectileManager);

        if (enemy->collide(*player)) {
            enemy->setCenter(previousX, previousY);


            if (player->canReceiveContactDamage()) {
                player->takeDamage(PLAYER_COLLISION_DAMAGE);
                player->startContactDamageCooldown();
            }

            if (enemy->canReceiveContactDamage()) {
                enemy->takeDamage(ENEMY_COLLISION_DAMAGE);
                enemy->startContactDamageCooldown();
            }
        }
    }
}