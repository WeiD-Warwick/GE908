#include "GEEnemyManager.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

namespace {
    constexpr int PLAYER_COLLISION_DAMAGE = 30;
    constexpr int ENEMY_COLLISION_DAMAGE = 30;
}

GEEnemyManager::GEEnemyManager() {
    _enemies.fillNull(MAX_ENEMIES);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    resetKillCounts();
}

GEEnemyManager::~GEEnemyManager() {

    // release all enemies
    _enemies.destroyAll();
    _enemies.clear();
}

void GEEnemyManager::load(GESaveData* saveData) {
    _saveData = saveData;
    _spawnInterval = DEFAULT_SPAWN_INTERVAL;
    _spawnTimer = 0.0f;
    _difficultyTimer = 0.0f;
    _elapsedTime = 0.0f;
    _activeEnemyCount = 0;

    _enemies.destroyAll();
    _enemies.fillNull(1000);
    resetKillCounts();
}

bool GEEnemyManager::spawnEnemyOutsideCamera(PlayerProvider& player) {
    if (_enemies.countActive() >= MAX_ENEMIES) return false;

    float camOffsetX = _saveData->getCameraOffsetX();
    float camOffsetY = _saveData->getCameraOffsetY();
    int screenW = _saveData->getScreenWidth();
    int screenH = _saveData->getScreenHeight();
    int mapW = _saveData->getMapTotalWidth();
    int mapH = _saveData->getMapTotalHeight();

    const int safeDistance = _saveData->getTileWidth() * 2;
    int side = rand() % 4;
    float x = 0, y = 0;

    switch (side) {
    case 0:
        // generat enemy at top
        x = camOffsetX + rand() % screenW;
        y = camOffsetY - safeDistance; 
        break;            
    case 1: 
        // generat enemy at bottom
        x = camOffsetX + rand() % screenW; 
        y = camOffsetY + screenH + safeDistance; 
        break;  
    case 2: 
        // generat enemy at left
        x = camOffsetX - safeDistance;
        y = camOffsetY + rand() % screenH;
        break;            
    case 3:
        // generat enemy at right
        x = camOffsetX + screenW + safeDistance;
        y = camOffsetY + rand() % screenH;
        break;  
    }

    x = clamp(x, 0.0f, static_cast<float>(mapW - 1));
    y = clamp(y, 0.0f, static_cast<float>(mapH - 1));

    GEEnemyType type = static_cast<GEEnemyType>(rand() % 4);
    GEEnemy* enemy = new GEEnemy(type);
    enemy->setMapBounds(mapW, mapH);
    enemy->setCenter(x, y);

    _enemies.add(enemy);
    ++_activeEnemyCount;
    return true;
}

void GEEnemyManager::draw(Window& window, const GECamera& camera) {
    _enemies.forEachActive([&](GEEnemy* enemy, unsigned int) {
        if (enemy && enemy->isAlive())
            enemy->draw(window, camera);
        });
}

void GEEnemyManager::update(float deltaTime, GEContext& ctx) {
    _spawnTimer += deltaTime;
    _difficultyTimer += deltaTime;
    _elapsedTime += deltaTime;

    const int capSteps = static_cast<int>(_elapsedTime / ACTIVE_ENEMY_CAP_STEP_TIME);
    const int activeEnemyCap = min(
        MAX_ENEMIES,
        BASE_ACTIVE_ENEMY_CAP + capSteps * ACTIVE_ENEMY_CAP_INCREMENT
    );

    if (_enemies.countActive() < activeEnemyCap) {
        while (_spawnTimer >= _spawnInterval) {
            if (!spawnEnemyOutsideCamera(ctx.playerProvider())) break;
            _spawnTimer -= _spawnInterval;
        }
    }

    if (_difficultyTimer >= SPAWN_DIFFICULTY_STEP_TIME) {
        _difficultyTimer -= SPAWN_DIFFICULTY_STEP_TIME;
        _spawnInterval = max(_spawnInterval - SPAWN_INTERVAL_STEP, MIN_SPAWN_INTERVAL);
    }

    GEPlayer& player = static_cast<GEPlayer&>(ctx.playerProvider());

    _enemies.forEachActive([&](GEEnemy* enemy, unsigned int) {
        if (!enemy) return;

        enemy->update(deltaTime, player.getCenterX(), player.getCenterY(), ctx);

        // detect collision
        if (enemy->collide(player)) {
            if (player.canReceiveContactDamage()) {
                player.takeDamage(PLAYER_COLLISION_DAMAGE);
                player.startContactDamageCooldown();
            }
            if (enemy->canReceiveContactDamage()) {
                enemy->takeDamage(ENEMY_COLLISION_DAMAGE);
                enemy->startContactDamageCooldown();
            }
        }

        // record kill
        if (!enemy->isAlive())
            registerEnemyKill(enemy->getType());
        });

    // deleate all died enemies
    _enemies.destroyInactive();
}

void GEEnemyManager::registerEnemyKill(GEEnemyType type) {
    int index = static_cast<int>(type);
    if (index >= 0 && index < ENEMY_TYPE_COUNT)
        ++_killCounts[index];
}

void GEEnemyManager::resetKillCounts() {
    for (int& k : _killCounts) k = 0;
}

int GEEnemyManager::getKillCount(GEEnemyType type) const {
    int index = static_cast<int>(type);
    return (index >= 0 && index < ENEMY_TYPE_COUNT) ? _killCounts[index] : 0;
}

void GEEnemyManager::removeEnemy(GEEnemy* e) {
    if (!e) return;
    delete e;
    _enemies.remove(e);
    if (_activeEnemyCount > 0) --_activeEnemyCount;
}
