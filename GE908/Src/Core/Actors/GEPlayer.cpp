#include <cmath>
#include "GEPlayer.h"
#include "BaseCharacter.h"
#include "GEEnemyManager.h"
#include "GEEnemy.h"
#include "../Maps/GEMapsManager.h"
#include "../Actors/GEProjectileManager.h"
#include "../../Foundation/GESaveData.h"


GEPlayer::GEPlayer()
	: BaseCharacter("", Player) {
	_hp = 200;
	_speed = 240;

    for (int i = 0;i < PLAYER_MAX_AOE_EFFECTS;i++) {
        _aoeEffects[i].active = false;
        _aoeEffects[i].centerX = 0.0f;
        _aoeEffects[i].centerY = 0.0f;
        _aoeEffects[i].radius = 0.0f;
        _aoeEffects[i].colorR = 255;
        _aoeEffects[i].colorG = 255;
        _aoeEffects[i].colorB = 255;
        _aoeEffects[i].remainingTime = 0.0f;
    }
}

GEPlayer::~GEPlayer() {}

void GEPlayer::bindWorldContext(const GEMapsManager* maps, const GEEnemyManager* enemies, GEProjectileManager* projectiles) {
    _mapsManager = maps;
    _enemyManager = enemies;
    _projectileManager = projectiles;
    _saveData = maps->getSaveData();

    int mapWorldWidth = _saveData->getMapTotalWidth();
    int mapWorldHeight = _saveData->getMapTotalHeight();

    _image.load("Src/Assets/Textures/player.png");
    setCenter(mapWorldWidth / 2.0f, mapWorldHeight / 2.0f);
    setMapBounds(mapWorldWidth, mapWorldHeight);
}

void GEPlayer::update(float deltaTime, Window& window) {
    float dirX = 0.0f;
    float dirY = 0.0f;

    if (window.keyPressed('W')) dirY -= 1.0f;
    if (window.keyPressed('S')) dirY += 1.0f;
    if (window.keyPressed('A')) dirX -= 1.0f;
    if (window.keyPressed('D')) dirX += 1.0f;

    if (window.keyPressed('Q')) _aoeKeyHeld = true;

    moveUpdate(deltaTime, dirX, dirY);

    autoAttack(deltaTime);
    aoeAttack(deltaTime);
}

static bool isWaterTile(int tileID) {
    return tileID >= 14 && tileID <= 22;
}

bool GEPlayer::collidesWithWater(float newX, float newY, const GEMapsManager& mapsManager) const {
    if (!_saveData) return false;

    int tileW = _saveData->getTileWidth();
    int tileH = _saveData->getTileHeight();
    int mapCols = _saveData->getMapColCount();
    int mapRows = _saveData->getMapRowCount();

    float playerCenterX = newX;
    float playerCenterY = newY;

    float playerHalfW = _image.width / 2.0f;
    float playerHalfH = _image.height / 2.0f;

    // tiles covered by player
    int centerTileX = static_cast<int>(playerCenterX / tileW);
    int centerTileY = static_cast<int>(playerCenterY / tileH);

    for (int dy = -1;dy <= 1;dy++) {
        for (int dx = -1;dx <= 1;dx++) {
            int tileX = centerTileX + dx;
            int tileY = centerTileY + dy;
            if (tileX < 0 || tileX >= mapCols || tileY < 0 || tileY >= mapRows) continue;

            int tileID = _saveData->getTileID(0, tileY, tileX);
            if (!isWaterTile(tileID)) continue;

            GETile* tile = mapsManager.getTile(tileID);
            if (!tile) continue;

            float tileCenterX = tileX * tileW + tileW / 2.0f;
            float tileCenterY = tileY * tileH + tileH / 2.0f;
            tile->setCenter(tileCenterX, tileCenterY);

            float dxCenter = std::abs(playerCenterX - tileCenterX);
            float dyCenter = std::abs(playerCenterY - tileCenterY);

            float combinedHalfW = playerHalfW + tileW / 2.0f;
            float combinedHalfH = playerHalfH + tileH / 2.0f;

            if (dxCenter < combinedHalfW && dyCenter < combinedHalfH)
                return true;
        }
    }

    return false;
}

bool GEPlayer::collidesWithEnemies(float newX, float newY, const GEEnemyManager& enemyManager) const {
    int enemyCount = enemyManager.getEnemyCount();
    for (int i = 0;i < enemyCount;i++) {
        GEEnemy* enemy = enemyManager.getEnemyAt(i);

        if (!enemy || !enemy->isAlive()) continue;

        if (this->collideAt(newX, newY, *enemy))
            return true;
    }
    return false;
}

bool GEPlayer::isBlockedAt(float x, float y) const {
    if (!_saveData) return false;

    if (collidesWithWater(x, y, *_mapsManager)) 
        return true;

    if (collidesWithEnemies(x, y, *_enemyManager))
        return true;

    return false;
}

void GEPlayer::applyMovementBounds(float& newX, float& newY) {
    float camW = _saveData->getScreenWidth();
    float camH = _saveData->getScreenHeight();
    float mapW = _saveData->getMapTotalWidth();
    float mapH = _saveData->getMapTotalHeight();

    float minCenterX = camW / 2.0f;
    float maxCenterX = mapW - camW / 2.0f;
    float minCenterY = camH / 2.0f;
    float maxCenterY = mapH - camH / 2.0f;


    newX = clamp(newX, minCenterX, maxCenterX);
    newY = clamp(newY, minCenterY, maxCenterY);
}

void GEPlayer::autoAttack(float deltaTime) {
    if (!_enemyManager || !_projectileManager) {
        return;
    }

    _autoAttackTimer += deltaTime;

    float interval = _autoAttackIntervalBase / _autoAttackSpeedMultiplier;

    if (_autoAttackTimer < interval) {
        return;
    }

    _autoAttackTimer -= interval;

    GEEnemy* nearest = nullptr;
    float nearestDistSq = FLT_MAX;

    int enemyCount = _enemyManager->getEnemyCount();
    for (int i = 0;i < enemyCount;i++) {
        GEEnemy* enemy = _enemyManager->getEnemyAt(i);
        if (!enemy || !enemy->isAlive()) continue;
        float dx = enemy->getCenterX() - getCenterX();
        float dy = enemy->getCenterY() - getCenterY();
        float distSq = dx * dx + dy * dy;
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = enemy;
        }
    }

    if (!nearest) {
        return;
    }

    float playerCenterX = getCenterX();
    float playerCenterY = getCenterY();
    float targetCenterX = nearest->getCenterX();
    float targetCenterY = nearest->getCenterY();
    float dirX = targetCenterX - playerCenterX;
    float dirY = targetCenterY - playerCenterY;
    float len = std::sqrt(dirX * dirX + dirY * dirY);
    if (len == 0.0f) {
        return;
    }
    dirX /= len;
    dirY /= len;

    _projectileManager->addProjectile(FromPlayer, playerCenterX, playerCenterY, dirX, dirY, PLAYER_PROJECTILE_SPEED, PLAYER_PROJECTILE_DAMAGE);
}

void GEPlayer::aoeAttack(float deltaTime) {
    updateAoeEffects(deltaTime);

    // calculate cooldonw
    if (_aoeCooldownTimer > 0.0f) {
        _aoeCooldownTimer = max(_aoeCooldownTimer - deltaTime, 0.0f);
    }

    // check user input
    if (_aoeKeyHeld && _aoeCooldownTimer <= 0.0f) {
        executeAoeSkill();
    }
    _aoeKeyHeld = false;
}

void GEPlayer::executeAoeSkill() {
    if (!_enemyManager) return;

    const float originX = getCenterX();
    const float originY = getCenterY();

    // get enemies near the player
    GEEnemy* nearby[PLAYER_MAX_AOE_TARGETS];
    int nearbyCount = findEnemiesWithinRadius(originX, originY, _aoeRadius, nearby, PLAYER_MAX_AOE_TARGETS);
    if (nearbyCount <= 0) return;

    // select top n HP enemies
    GEEnemy* topTargets[PLAYER_MAX_AOE_TARGETS];
    int topCount = selectTopEnemiesByHP(nearby, nearbyCount, _aoeTargetCount, topTargets);
    if (topCount <= 0) return;

    // show range of aoe
    const unsigned char RANGE_COLOR[3] = { 0, 0, 255 };// blue
    spawnAoeEffect(originX, originY, _aoeRadius, RANGE_COLOR[0], RANGE_COLOR[1], RANGE_COLOR[2]);

    // show hit vfx of aoe
    const unsigned char IMPACT_COLOR[3] = { 255, 0, 0 };// red
    const float IMPACT_RADIUS = 30.0f;

    for (int i = 0;i < topCount;++i) {
        GEEnemy* enemy = topTargets[i];
        enemy->takeDamage(_aoeDamage);
        spawnAoeEffect(enemy->getCenterX(), enemy->getCenterY(), IMPACT_RADIUS,
            IMPACT_COLOR[0], IMPACT_COLOR[1], IMPACT_COLOR[2]);
    }

    // reset skill cooldown time
    _aoeCooldownTimer = _aoeCooldown;
}

int GEPlayer::findEnemiesWithinRadius(float cx, float cy, float radius, GEEnemy** outList, int maxCount) const {
    if (!_enemyManager) return 0;

    const float radiusSq = radius * radius;
    int count = 0;
    const int enemyCount = _enemyManager->getEnemyCount();

    for (int i = 0;i < enemyCount;++i) {
        GEEnemy* e = _enemyManager->getEnemyAt(i);
        if (!e || !e->isAlive()) continue;

        float dx = e->getCenterX() - cx;
        float dy = e->getCenterY() - cy;
        if (dx * dx + dy * dy <= radiusSq) {
            if (count < maxCount) {
                outList[count++] = e;
            }
        }
    }
    return count;
}

int GEPlayer::selectTopEnemiesByHP(GEEnemy** input, int count, int topN, GEEnemy** output) const {
    if (count == 0 || topN <= 0) return 0;

    // Sort
    for (int i = 0;i < count - 1;++i) {
        for (int j = i + 1;j < count;++j) {
            if (input[j]->getHP() > input[i]->getHP()) {
                std::swap(input[i], input[j]);
            }
        }
    }

    const int resultCount = min(topN, count);
    for (int i = 0;i < resultCount;++i) {
        output[i] = input[i];
    }
    return resultCount;
}

void GEPlayer::updateAoeEffects(float deltaTime) {
    for (int i = 0;i < PLAYER_MAX_AOE_EFFECTS;i++) {
        if (!_aoeEffects[i].active) continue;
        _aoeEffects[i].remainingTime -= deltaTime;
        if (_aoeEffects[i].remainingTime <= 0.0f) {
            _aoeEffects[i].active = false;
            _aoeEffects[i].remainingTime = 0.0f;
        }
    }
}

void GEPlayer::spawnAoeEffect(float centerX, float centerY, float radius, unsigned char r, unsigned char g, unsigned char b) {
    int slot = -1;
    for (int i = 0;i < PLAYER_MAX_AOE_EFFECTS;i++) {
        if (!_aoeEffects[i].active) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        float minTime = _aoeEffects[0].remainingTime;
        slot = 0;
        for (int i = 1;i < PLAYER_MAX_AOE_EFFECTS;i++) {
            if (_aoeEffects[i].remainingTime < minTime) {
                minTime = _aoeEffects[i].remainingTime;
                slot = i;
            }
        }
    }

    _aoeEffects[slot].active = true;
    _aoeEffects[slot].centerX = centerX;
    _aoeEffects[slot].centerY = centerY;
    _aoeEffects[slot].radius = radius;
    _aoeEffects[slot].colorR = r;
    _aoeEffects[slot].colorG = g;
    _aoeEffects[slot].colorB = b;
    _aoeEffects[slot].remainingTime = _aoeEffectDuration;
}

void GEPlayer::drawAoeIndicator(Window& window, const GECamera& camera) const {
    if (!SHOW_PLAYER_AOE_INDICATOR) {
        return;
    }

    const bool ready = _aoeCooldownTimer <= 0.0f;
    const unsigned char readyR = 0;
    const unsigned char readyG = 200;
    const unsigned char readyB = 255;
    const unsigned char cooldownTint = 90;

    if (ready) {
        drawCircle(window, camera, getCenterX(), getCenterY(), _aoeRadius, readyR, readyG, readyB);
    }
    else {
        drawCircle(window, camera, getCenterX(), getCenterY(), _aoeRadius, cooldownTint, cooldownTint, cooldownTint);
    }
}

void GEPlayer::drawAoeEffects(Window& window, const GECamera& camera) const {
    for (int i = 0;i < PLAYER_MAX_AOE_EFFECTS;i++) {
        if (!_aoeEffects[i].active) continue;
        drawCircle(window, camera, _aoeEffects[i].centerX, _aoeEffects[i].centerY, _aoeEffects[i].radius, _aoeEffects[i].colorR, _aoeEffects[i].colorG, _aoeEffects[i].colorB);
    }
}

void GEPlayer::drawCircle(Window& window, const GECamera& camera, float centerX, float centerY, float radius, unsigned char r, unsigned char g, unsigned char b) const {
    int camX = camera.getX();
    int camY = camera.getY();

    int winW = window.getWidth();
    int winH = window.getHeight();

    int radiusInt = static_cast<int>(radius);
    if (radiusInt <= 0) {
        return;
    }
    int cx = static_cast<int>(centerX - camX);
    int cy = static_cast<int>(centerY - camY);
    int radiusSq = radiusInt * radiusInt;

    for (int dx = -radiusInt;dx <= radiusInt;++dx) {
        int rem = radiusSq - dx * dx;
        if (rem < 0) continue;
        int dy = static_cast<int>(std::sqrt(static_cast<float>(rem)));

        int x = cx + dx;
        if (x < 0 || x >= winW) continue;

        int y1 = cy + dy;
        int y2 = cy - dy;
        if (y1 >= 0 && y1 < winH) window.draw(x, y1, r, g, b);
        if (y2 >= 0 && y2 < winH) window.draw(x, y2, r, g, b);
    }
}

void GEPlayer::draw(Window& window, const GECamera& camera) {
    GECollisible::draw(window, camera);
    drawAoeIndicator(window, camera);
    drawAoeEffects(window, camera);
}