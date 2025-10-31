#include "GEEnemyManager.h"
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

    int camX = _saveData->getCameraOffsetX();
    int camY = _saveData->getCameraOffsetY();
    int camW = _saveData->getWindowWidth();
    int camH = _saveData->getWindowHeight();
    int mapW = _saveData->getMapTotalWidth();
    int mapH = _saveData->getMapTotalHeight();

    const int margin = 64;
    int side = rand() % 4;
    int x = 0, y = 0;

    switch (side) {
    case 0: x = camX + rand() % camW; y = camY - margin; break;
    case 1: x = camX + rand() % camW; y = camY + camH + margin; break;
    case 2: x = camX - margin;        y = camY + rand() % camH; break;
    case 3: x = camX + camW + margin; y = camY + rand() % camH; break;
    }

    // clamp ?????
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > mapW - 1) x = mapW - 1;
    if (y > mapH - 1) y = mapH - 1;

    GEEnemyType type = static_cast<GEEnemyType>(rand() % 4);
    GEEnemy* e = new GEEnemy(type);
    e->setMapBounds(mapW, mapH);
    e->setPosition(x, y);

    _enemies[_enemyCount++] = e;
}

void GEEnemyManager::draw(GEWindow& window, const GECamera& camera) {
    for (unsigned int i = 0; i < _enemyCount; i++) {
        if (_enemies[i])
            _enemies[i]->draw(window, camera);
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

    for (unsigned int i = 0; i < _enemyCount; i++) {
        if (_enemies[i])
            _enemies[i]->update(deltaTime, player->getX(), player->getY());
    }
}