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

    if (camW <= 0 || camH <= 0) {
        GELog::shared().warning("Invalid camera size, spawn cancelled.");
        return;
    }

    int spawnX, spawnY;
    int side = rand() % 4;

    switch (side) {
    case 0: spawnX = camX + rand() % camW; spawnY = camY - 100; break;
    case 1: spawnX = camX + rand() % camW; spawnY = camY + camH + 100; break;
    case 2: spawnX = camX - 100; spawnY = camY + rand() % camH; break;
    case 3: spawnX = camX + camW + 100; spawnY = camY + rand() % camH; break;
    }

    GEEnemyType type = static_cast<GEEnemyType>(rand() % 4);
    _enemies[_enemyCount] = new GEEnemy(type);
    _enemies[_enemyCount]->setPosition(spawnX, spawnY);
    _enemyCount++;
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

void GEEnemyManager::draw(GEWindow& window) {
    for (unsigned int i = 0; i < _enemyCount; i++) {
        if (_enemies[i])
            _enemies[i]->draw(window);
    }
}
