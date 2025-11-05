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

static int clamp(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

void GEPlayer::loadData(GESaveData* saveData) {
	_saveData = saveData;

	int mapWorldWidth = _saveData->getMapTotalWidth();
	int mapWorldHeight = _saveData->getMapTotalHeight();

	int playerStartX = static_cast<int>((mapWorldWidth / 2.0f) - (_width / 2.0f));
	int playerStartY = static_cast<int>((mapWorldHeight / 2.0f) - (_height / 2.0f));

	setPosition(playerStartX, playerStartY);
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

            int tileWorldX = tileX * tileW;
            int tileWorldY = tileY * tileH;
            tile->setPosition(tileWorldX, tileWorldY);

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
    const GEMapsManager& mapsManager, const GEEnemyManager& enemyManager)
{
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

    int minX = (mapW > camW) ? camW / 2 - _width / 2 : mapW / 2 - _width / 2;
    int maxX = (mapW > camW) ? mapW - camW / 2 - _width / 2 : mapW / 2 - _width / 2;
    int minY = (mapH > camH) ? camH / 2 - _height / 2 : mapH / 2 - _height / 2;
    int maxY = (mapH > camH) ? mapH - camH / 2 - _height / 2 : mapH / 2 - _height / 2;

    const int COLLISION_MARGIN = 2;

    // X
    if (deltaX != 0) {
        int proposedX = clamp(_originX + deltaX, minX, maxX);

        // loose boundary detection
        bool blockedX = collidesWithWater(proposedX, _originY, mapsManager)
            || collidesWithEnemies(proposedX, _originY, enemyManager);

        if (!blockedX) {
            _originX = proposedX;
        }
        else {
            // debounce
            int stepX = (deltaX > 0) ? 1 : -1;
            for (int x = _originX; x != proposedX; x += stepX) {
                if (collidesWithWater(x, _originY, mapsManager)
                    || collidesWithEnemies(x, _originY, enemyManager)) {
                    _originX = x - stepX * COLLISION_MARGIN;
                    break;
                }
                _originX = x;
            }
        }
    }

    // Y
    if (deltaY != 0) {
        int proposedY = clamp(_originY + deltaY, minY, maxY);

        bool blockedY = collidesWithWater(_originX, proposedY, mapsManager)
            || collidesWithEnemies(_originX, proposedY, enemyManager);

        if (!blockedY) {
            _originY = proposedY;
        }
        else {
            int stepY = (deltaY > 0) ? 1 : -1;
            for (int y = _originY; y != proposedY; y += stepY) {
                if (collidesWithWater(_originX, y, mapsManager)
                    || collidesWithEnemies(_originX, y, enemyManager)) {
                    _originY = y - stepY * COLLISION_MARGIN;
                    break;
                }
                _originY = y;
            }
        }
    }

    _originX = clamp(_originX, minX, maxX);
    _originY = clamp(_originY, minY, maxY);
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

        int dx = enemy->getCenterX() - getCenterX();
        int dy = enemy->getCenterY() - getCenterY();

        float distSq = static_cast<float>(dx * dx + dy * dy);

        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = enemy;
        }
    }

    if (!nearest) return;

    int px = getCenterX();
    int py = getCenterY();
    int tx = nearest->getCenterX();
    int ty = nearest->getCenterY();
    float dirX = static_cast<float>(tx - px);
    float dirY = static_cast<float>(ty - py);
    float len = sqrtf(dirX * dirX + dirY * dirY);
    if (len == 0) return;
    dirX /= len; dirY /= len;

    projectileManager.addProjectile(FromPlayer, px, py, dirX, dirY, 100.0f, 200);
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
