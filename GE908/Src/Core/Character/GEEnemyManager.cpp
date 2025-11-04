#include "GEEnemyManager.h"
#include "GEPlayer.h"
#include <cstdlib>
#include <ctime>

GEEnemyManager::GEEnemyManager() {
    for (int i = 0; i < MAX_ENEMIES; i++) _enemies[i] = nullptr;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

GEEnemyManager::~GEEnemyManager() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (_enemies[i]) {
            delete _enemies[i];
            _enemies[i] = nullptr;
        }
    }
}

void GEEnemyManager::load(GESaveData* saveData) {
    _saveData = saveData;
}

void GEEnemyManager::spawnEnemyOutsideCamera(GEPlayer* player) {
    if (_enemyCount >= MAX_ENEMIES) return;

    int camOffsetX = _saveData->getCameraOffsetX();
    int camOffsetY = _saveData->getCameraOffsetY();
    int screenW = _saveData->getScreenWidth();
    int screenH = _saveData->getScreenHeight();
    int mapW = _saveData->getMapTotalWidth();
    int mapH = _saveData->getMapTotalHeight();

    // make the enemy appearance more natural
    const int safeDistance = _saveData->getTileWidth() * 2;
    int side = rand() % 4;
    int x = 0, y = 0;

    switch (side) {
    case 0:
        // top
        x = camOffsetX + rand() % screenW;
        y = camOffsetY - safeDistance; break;
    case 1: 
        // bottom
        x = camOffsetX + rand() % screenW;
        y = camOffsetY + screenH + safeDistance; break;
    case 2: 
        // left
        x = camOffsetX - safeDistance;
        y = camOffsetY + rand() % screenH; break;
    case 3: 
        // right
        x = camOffsetX + screenW + safeDistance;
        y = camOffsetY + rand() % screenH; break;
    }

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > mapW - 1) x = mapW - 1;
    if (y > mapH - 1) y = mapH - 1;

    GEEnemyType type = static_cast<GEEnemyType>(rand() % 4);
    GEEnemy* enemy = new GEEnemy(type);
    enemy->setMapBounds(mapW, mapH);
    enemy->setPosition(x, y);

    _enemies[_enemyCount++] = enemy;
}

void GEEnemyManager::draw(GEWindow& window, const GECamera& camera) {
    for (unsigned int i = 0; i < _enemyCount; i++) {
        GEEnemy* enemy = _enemies[i];
        if (!enemy || !enemy->isAlive()) continue;
        enemy->draw(window, camera);
    }
}

void GEEnemyManager::update(float deltaTime, GEPlayer* player) {
    _spawnTimer += deltaTime;
    if (_spawnTimer > _spawnInterval) {
        spawnEnemyOutsideCamera(player);
        _spawnTimer = 0.0f;
        _spawnInterval -= 0.2f;
        if (_spawnInterval < 0.5f) _spawnInterval = 0.5f;
    }

    for (int i = 0; i < _enemyCount; i++) {
        GEEnemy* enemy = _enemies[i];
        if (!enemy || !enemy->isAlive()) continue;

        int previousX = enemy->getX();
        int previousY = enemy->getY();

        enemy->update(deltaTime, player->getX(), player->getY());

        if (enemy->collide(*player)) {
            enemy->setPosition(previousX, previousY);
        }
    }
}