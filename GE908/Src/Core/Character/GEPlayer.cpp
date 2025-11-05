#include "GEPlayer.h"
#include "BaseCharacter.h"
#include "GEEnemyManager.h"
#include "../../Foundation/GESaveData.h"
#include "../Maps/GEMapsManager.h"
#include "../Items/GEProjectileManager.h"
#include "../Items/GEBuffManager.h"
#include <algorithm>

GEPlayer::GEPlayer()
	: BaseCharacter("Src/Assets/Textures/player.png", Player) {
	_hp = 100;
	_speed = 200;
}

GEPlayer::~GEPlayer() {}

static float clamp(float value, float minVal, float maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

void GEPlayer::loadData(GESaveData* saveData) {
	_saveData = saveData;

	int mapWorldWidth = _saveData->getMapTotalWidth();
	int mapWorldHeight = _saveData->getMapTotalHeight();

	setCenter(mapWorldWidth / 2.0f, mapWorldHeight / 2.0f);
	setMapBounds(mapWorldWidth, mapWorldHeight);
}

static bool isWaterTile(int tileID) {
    return tileID >= 14 && tileID <= 22;
}

bool GEPlayer::collidesWithWater(int newX, int newY, const GEMapsManager& mapsManager) const {
    if (!_saveData) return false;

    int tileW = _saveData->getTileWidth();
    int tileH = _saveData->getTileHeight();
    int mapCols = _saveData->getMapColCount();
    int mapRows = _saveData->getMapRowCount();

    int centerTileX = newX / tileW;
    int centerTileY = newY / tileH;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int tileX = centerTileX + dx;
            int tileY = centerTileY + dy;
            if (tileX < 0 || tileX >= mapCols || tileY < 0 || tileY >= mapRows) continue;

            int tileID = _saveData->getTileID(0, tileY, tileX);

            if (!isWaterTile(tileID)) continue;

            GETile* tile = mapsManager.getTile(tileID);
            if (!tile) continue;

            float tileWorldX = tileX * tileW + (tileW / 2.0f);
            float tileWorldY = tileY * tileH + (tileH / 2.0f);
            
            tile->setCenter(tileWorldX, tileWorldY);

            if (this->collideAt(newX, newY, *tile))
                return true;
        }
    }

    return false;
}


bool GEPlayer::collidesWithEnemies(int newX, int newY, const GEEnemyManager& enemyManager) const {
    int enemyCount = enemyManager.getEnemyCount();
    for (int i = 0; i < enemyCount; i++) {
        GEEnemy* enemy = enemyManager.getEnemyAt(i);
        if (!enemy) continue;
        if (this->collideAt(newX, newY, *enemy))
            return true;
    }
    return false;
}

void GEPlayer::update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight,
    const GEMapsManager& mapsManager, const GEEnemyManager& enemyManager) {
    if (!_saveData) return;

    float moveDelta = _speed * deltaTime;
    int moveAmount = static_cast<int>(moveDelta > 1.0f ? moveDelta : 1.0f);

    int deltaX = 0;
    int deltaY = 0;

    if (moveUp)    deltaY -= moveAmount;
    if (moveDown)  deltaY += moveAmount;
    if (moveLeft)  deltaX -= moveAmount;
    if (moveRight) deltaX += moveAmount;

    if (deltaX == 0 && deltaY == 0) return;

    int camW = _saveData->getScreenWidth();
    int camH = _saveData->getScreenHeight();
    int mapW = _mapWidth;
    int mapH = _mapHeight;

    float minCenterX = camW / 2.0f;
    float maxCenterX = mapW - camW / 2.0f;
    float minCenterY = camH / 2.0f;
    float maxCenterY = mapH - camH / 2.0f;

    const int COLLISION_MARGIN = 2; 

    float newCenterX = getCenterX();
    float newCenterY = getCenterY();

    // deal X axis
    if (deltaX != 0) {
        float nextCenterX = clamp(getCenterX() + deltaX, minCenterX, maxCenterX);

        // loose boundary detection
        bool blockedX = collidesWithWater(nextCenterX, getCenterY(), mapsManager) || collidesWithEnemies(nextCenterX, getCenterY(), enemyManager);

        if (blockedX) {
            // debounce, back to check if still have collision
            int backStepX = (deltaX > 0) ? 1 : -1;
            for (int x = getCenterX(); x != nextCenterX; x += backStepX) {
                if (collidesWithWater(x, getCenterY(), mapsManager) || collidesWithEnemies(x, getCenterY(), enemyManager)) {
                    newCenterX = x - backStepX * COLLISION_MARGIN;
                    break;
                }
                newCenterX = x;
            }
        }
        else {
            newCenterX = nextCenterX;
        }
    }

    // deal Y axis
    if (deltaY != 0) {
        float nextCenterY = clamp(getCenterY() + deltaY, minCenterY, maxCenterY);

        bool blockedY = collidesWithWater(getCenterX(), nextCenterY, mapsManager) || collidesWithEnemies(getCenterX(), nextCenterY, enemyManager);

        if (blockedY) {
            int backStepY = (deltaY > 0) ? 1 : -1;
            for (int y = getCenterY(); y != nextCenterY; y += backStepY) {
                if (collidesWithWater(getCenterX(), y, mapsManager) || collidesWithEnemies(getCenterX(), y, enemyManager)) {
                    newCenterY = y - backStepY * COLLISION_MARGIN;
                    break;
                }
                newCenterY = y;
            }
        }
        else {
            newCenterY = nextCenterY;

        }
    }

    newCenterX = clamp(newCenterX, minCenterX, maxCenterX);
    newCenterY = clamp(newCenterY, minCenterY, maxCenterY);

    setCenter(newCenterX, newCenterY);
}

void GEPlayer::updateAttack(float deltaTime, const GEEnemyManager& enemyManager, GEProjectileManager& projectileManager) {
    _attackTimer += deltaTime;

    if (_attackTimer < getEffectiveAttackInterval()) return;

    _attackTimer = 0.0f;

    GEEnemy* nearest = nullptr;
    float nearestDistSq = 99999999.0f;

    int enemyCount = enemyManager.getEnemyCount();

    for (int i = 0; i < enemyCount; i++) {

        GEEnemy* enemy = enemyManager.getEnemyAt(i);
        if (!enemy || !enemy->isAlive()) continue;

        float distanceX = enemy->getCenterX() - getCenterX();
        float distanceY = enemy->getCenterY() - getCenterY();

        float distSq = distanceX * distanceX + distanceY * distanceY;

        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = enemy;
        }
    }

    if (!nearest) return;

    float playerCenterX = getCenterX();
    float playerCenterY = getCenterY();
    float targetCenterX = nearest->getCenterX();
    float targetCenterY = nearest->getCenterY();
    float dirX = targetCenterX - playerCenterX;
    float dirY = targetCenterY - playerCenterY;
    float len = sqrtf(dirX * dirX + dirY * dirY);

    if (len == 0) return;

    dirX /= len; dirY /= len;

    projectileManager.addProjectile(FromPlayer, playerCenterX, playerCenterY, dirX, dirY, 100.0f, 200);
}

void GEPlayer::updateSkill(float deltaTime, bool triggerSkill, GEEnemyManager& enemyManager) {
    _skillTimer += deltaTime;

    if (!triggerSkill) return;

    if (_skillTimer < _skillCooldown) return;

    _skillTimer = 0.0f;

    int targetCount = _baseSkillTargetCount + _bonusSkillTargets;
    if (targetCount <= 0) return;

    GEEnemy* aliveEnemies[MAX_ENEMIES];
    bool selected[MAX_ENEMIES];
    int aliveCount = 0;

    int enemyCount = enemyManager.getEnemyCount();
    for (int i = 0; i < enemyCount && aliveCount < MAX_ENEMIES; ++i) {
        GEEnemy* enemy = enemyManager.getEnemyAt(i);
        if (enemy && enemy->isAlive()) {
            aliveEnemies[aliveCount] = enemy;
            selected[aliveCount] = false;
            ++aliveCount;
        }
    }

    if (aliveCount == 0) return;

    float radiusSq = _skillRadius * _skillRadius;
    int limit = targetCount < aliveCount ? targetCount : aliveCount;

    for (int targetIndex = 0; targetIndex < limit; ++targetIndex) {
        int bestIndex = -1;
        int bestHP = -1;

        for (int i = 0; i < aliveCount; ++i) {
            if (selected[i]) continue;
            int hp = aliveEnemies[i]->getHP();
            if (hp > bestHP) {
                bestHP = hp;
                bestIndex = i;
            }
        }

        if (bestIndex == -1) {
            break;
        }

        selected[bestIndex] = true;
        GEEnemy* target = aliveEnemies[bestIndex];
        int tx = target->getCenterX();
        int ty = target->getCenterY();

        for (int i = 0; i < aliveCount; ++i) {
            GEEnemy* enemy = aliveEnemies[i];
            int dx = enemy->getCenterX() - tx;
            int dy = enemy->getCenterY() - ty;
            float distSq = static_cast<float>(dx * dx + dy * dy);
            if (distSq <= radiusSq) {
                enemy->takeDamage(_skillDamage);
            }
        }
    }
}

void GEPlayer::applyBuff(GEBuffType type) {
    switch (type) {
    case GEBuffType::AttackSpeed:
        _attackSpeedMultiplier = _attackSpeedMultiplier > 1.8f ? _attackSpeedMultiplier : 1.8f;
        _attackSpeedBuffTimer = 10.0f;
        break;
    case GEBuffType::SkillTargets:
        _bonusSkillTargets = _bonusSkillTargets > 2 ? _bonusSkillTargets : 2;
        _skillBuffTimer = 10.0f;
        break;
    }
}

void GEPlayer::updateBuffTimers(float deltaTime) {
    if (_attackSpeedBuffTimer > 0.0f) {
        _attackSpeedBuffTimer -= deltaTime;
        if (_attackSpeedBuffTimer <= 0.0f) {
            _attackSpeedBuffTimer = 0.0f;
            _attackSpeedMultiplier = 1.0f;
        }
    }

    if (_skillBuffTimer > 0.0f) {
        _skillBuffTimer -= deltaTime;
        if (_skillBuffTimer <= 0.0f) {
            _skillBuffTimer = 0.0f;
            _bonusSkillTargets = 0;
        }
    }
}

float GEPlayer::getEffectiveAttackInterval() const {
    float multiplier = (_attackSpeedMultiplier > 0.0f) ? _attackSpeedMultiplier : 1.0f;
    return _attackIntervalBase / multiplier;
}
