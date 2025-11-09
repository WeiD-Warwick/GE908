#include "GEEnemyManager.h"
#include <cstdlib>
#include <ctime>

GEEnemyManager::GEEnemyManager() {
    _enemies.fillNull(Enemy::MAX_ENEMIES);
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
    _spawnInterval = Enemy::DEFAULT_SPAWN_INTERVAL;
    _spawnTimer = 0.0f;
    _difficultyTimer = 0.0f;
    _elapsedTime = 0.0f;
    _activeEnemyCount = 0;

    _enemies.destroyAll();
    _enemies.fillNull(Enemy::MAX_ENEMIES);
    resetKillCounts();

    if (_saveData) {
        if (const GEEnemyManagerState* savedState = _saveData->getEnemyManagerState()) {
            applyState(*savedState);
        }
    }
}

bool GEEnemyManager::spawnEnemyOutsideCamera(PlayerProvider& player) {
    if (_enemies.countActive() >= Enemy::MAX_ENEMIES) return false;

    float camOffsetX = _saveData->getCameraOffsetX();
    float camOffsetY = _saveData->getCameraOffsetY();
    int screenW = _saveData->getScreenWidth();
    int screenH = _saveData->getScreenHeight();
    int mapW = _saveData->getActiveChunkPixelWidth();
    int mapH = _saveData->getActiveChunkPixelHeight();
    const bool infinite = _saveData->isInfiniteMap();

    if (!infinite && (mapW <= 0 || mapH <= 0)) return false;

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

    if (!infinite) {
        x = clamp(x, 0.0f, static_cast<float>(mapW - 1));
        y = clamp(y, 0.0f, static_cast<float>(mapH - 1));
    }

    GEEnemyType type = static_cast<GEEnemyType>(rand() % 4);
    GEEnemy* enemy = new GEEnemy(type);

    if (infinite)
        enemy->setMapBounds(-1, -1);
    else
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

    const int capSteps = static_cast<int>(_elapsedTime / Enemy::ACTIVE_ENEMY_CAP_STEP_TIME);
    const int activeEnemyCap = min(
        Enemy::MAX_ENEMIES,
        Enemy::BASE_ACTIVE_ENEMY_CAP + capSteps * Enemy::ACTIVE_ENEMY_CAP_INCREMENT
    );

    if (_enemies.countActive() < activeEnemyCap) {
        while (_spawnTimer >= _spawnInterval) {
            if (!spawnEnemyOutsideCamera(ctx.playerProvider())) break;
            _spawnTimer -= _spawnInterval;
        }
    }

    if (_difficultyTimer >= Enemy::SPAWN_DIFFICULTY_STEP_TIME) {
        _difficultyTimer -= Enemy::SPAWN_DIFFICULTY_STEP_TIME;
        _spawnInterval = max(_spawnInterval - Enemy::SPAWN_INTERVAL_STEP, Enemy::MIN_SPAWN_INTERVAL);
    }

    GEPlayer& player = static_cast<GEPlayer&>(ctx.playerProvider());

    _enemies.forEachActive([&](GEEnemy* enemy, unsigned int) {
        if (!enemy) return;

        enemy->update(deltaTime, player.getCenterX(), player.getCenterY(), ctx);

        // detect collision
        if (enemy->collide(player)) {
            if (player.canReceiveContactDamage()) {
                player.takeDamage(Enemy::PLAYER_COLLISION_DAMAGE);
                player.startContactDamageCooldown();
            }
            if (enemy->canReceiveContactDamage()) {
                enemy->takeDamage(Enemy::ENEMY_COLLISION_DAMAGE);
                enemy->startContactDamageCooldown();
            }
        }

        // record kill
        if (!enemy->isAlive())
            registerEnemyKill(enemy->getType());
        });

    // deleate all died enemies
    _enemies.destroyInactive();

    _activeEnemyCount = _enemies.countActive();

    if (_saveData) {
        // keep active chunk information current for manual saves
        const float playerX = player.getCenterX();
        const float playerY = player.getCenterY();
        _saveData->updateActiveChunkFromWorldPosition(playerX, playerY);
    }
}

void GEEnemyManager::registerEnemyKill(GEEnemyType type) {
    int index = static_cast<int>(type);
    if (index >= 0 && index < Enemy::ENEMY_TYPE_COUNT)
        ++_killCounts[index];
}

void GEEnemyManager::resetKillCounts() {
    for (int& k : _killCounts) k = 0;
}

int GEEnemyManager::getKillCount(GEEnemyType type) const {
    int index = static_cast<int>(type);
    return (index >= 0 && index < Enemy::ENEMY_TYPE_COUNT) ? _killCounts[index] : 0;
}

void GEEnemyManager::removeEnemy(GEEnemy* e) {
    if (!e) return;
    delete e;
    _enemies.remove(e);
    if (_activeEnemyCount > 0) --_activeEnemyCount;
}

GEEnemyManagerState GEEnemyManager::snapshotState() const {
    GEEnemyManagerState state;
    state.spawnTimer = _spawnTimer;
    state.spawnInterval = _spawnInterval;
    state.difficultyTimer = _difficultyTimer;
    state.elapsedTime = _elapsedTime;
    for (int i = 0; i < Enemy::ENEMY_TYPE_COUNT; ++i)
        state.killCounts[static_cast<size_t>(i)] = _killCounts[i];

    _enemies.forEachActive([&](GEEnemy* enemy, unsigned int) {
        if (!enemy || !enemy->isAlive()) return;
        GEEnemyState enemyState = enemy->snapshotState();
        state.addEnemyState(enemyState);
        });
    return state;
}

void GEEnemyManager::applyState(const GEEnemyManagerState& state) {
    _spawnTimer = state.spawnTimer;
    _spawnInterval = state.spawnInterval > 0.0f ? state.spawnInterval : Enemy::DEFAULT_SPAWN_INTERVAL;
    _difficultyTimer = state.difficultyTimer;
    _elapsedTime = state.elapsedTime;

    resetKillCounts();
    for (int i = 0; i < Enemy::ENEMY_TYPE_COUNT; ++i)
        _killCounts[i] = state.killCounts[static_cast<size_t>(i)];

    _enemies.destroyAll();
    _enemies.fillNull(Enemy::MAX_ENEMIES);

    const bool infinite = _saveData && _saveData->isInfiniteMap();
    const int mapW = _saveData ? _saveData->getActiveChunkPixelWidth() : 0;
    const int mapH = _saveData ? _saveData->getActiveChunkPixelHeight() : 0;

    state.forEachEnemyState([&](const GEEnemyState& enemyState) {
        if (enemyState.hp <= 0) return;
        GEEnemy* enemy = new GEEnemy(enemyState.type);
        if (infinite)
            enemy->setMapBounds(-1, -1);
        else
            enemy->setMapBounds(mapW, mapH);
        enemy->applyState(enemyState);
        _enemies.add(enemy);
        });

    _activeEnemyCount = _enemies.countActive();
}