#include "GEPlayer.h"
#include "BaseCharacter.h"
#include "GEEnemyManager.h"
#include "../Maps/GEMapsManager.h"
#include "../Items/GEProjectileManager.h"
#include "../../Foundation/GESaveData.h"

GEPlayer::GEPlayer()
	: BaseCharacter("", Player) {
	_hp = 100;
	_speed = 200;

    for (int i = 0; i < PLAYER_MAX_AOE_EFFECTS; i++) {
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

void GEPlayer::update(float deltaTime, GEWindow& window) {
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
    for (int i = 0; i < enemyCount; i++) {
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

    _projectileManager->addProjectile(FromPlayer, playerCenterX, playerCenterY, dirX, dirY, 100.0f, 200);
}

void GEPlayer::aoeAttack(float deltaTime) {
    updateAoeEffects(deltaTime);

    if (_aoeCooldownTimer > 0.0f) {
        _aoeCooldownTimer -= deltaTime;
        if (_aoeCooldownTimer < 0.0f) {
            _aoeCooldownTimer = 0.0f;
        }
    }

    if (_aoeKeyHeld && _aoeCooldownTimer <= 0.0f) {
        castAoeSkill();
    }

    _aoeKeyHeld = false;
}

void GEPlayer::castAoeSkill() {
    if (!_enemyManager) {
        return;
    }

    int maxTargets = _aoeTargetCount;
    if (maxTargets > PLAYER_MAX_AOE_TARGETS) {
        maxTargets = PLAYER_MAX_AOE_TARGETS;
    }

    if (maxTargets <= 0) {
        return;
    }

    GEEnemy* selected[PLAYER_MAX_AOE_TARGETS];
    int selectedHP[PLAYER_MAX_AOE_TARGETS];
    int selectedCount = 0;

    float radiusSq = _aoeRadius * _aoeRadius;

    int enemyCount = _enemyManager->getEnemyCount();
    for (int i = 0; i < enemyCount; i++) {
        GEEnemy* enemy = _enemyManager->getEnemyAt(i);
        if (!enemy || !enemy->isAlive()) continue;

        float dx = enemy->getCenterX() - getCenterX();
        float dy = enemy->getCenterY() - getCenterY();
        float distSq = dx * dx + dy * dy;
        if (distSq > radiusSq) {
            continue;
        }

        int hp = enemy->getHP();

        int insertIndex = selectedCount;
        for (int j = 0; j < selectedCount; j++) {
            if (hp > selectedHP[j]) {
                insertIndex = j;
                break;
            }
        }

        if (selectedCount < maxTargets) {
            for (int j = selectedCount; j > insertIndex; --j) {
                selected[j] = selected[j - 1];
                selectedHP[j] = selectedHP[j - 1];
            }
            selected[insertIndex] = enemy;
            selectedHP[insertIndex] = hp;
            selectedCount++;
        }
        else if (insertIndex < maxTargets) {
            for (int j = maxTargets - 1; j > insertIndex; --j) {
                selected[j] = selected[j - 1];
                selectedHP[j] = selectedHP[j - 1];
            }
            selected[insertIndex] = enemy;
            selectedHP[insertIndex] = hp;
        }
    }

    if (selectedCount == 0) {
        return;
    }

    for (int i = 0; i < selectedCount; i++) {
        GEEnemy* enemy = selected[i];
        enemy->takeDamage(_aoeDamage);
        spawnAoeEffect(enemy->getCenterX(), enemy->getCenterY(), 30, 255, 140, 0);
    }

    spawnAoeEffect(getCenterX(), getCenterY(), _aoeRadius, 0, 200, 255);
    _aoeCooldownTimer = _aoeCooldown;
}

void GEPlayer::updateAoeEffects(float deltaTime) {
    for (int i = 0; i < PLAYER_MAX_AOE_EFFECTS; i++) {
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
    for (int i = 0; i < PLAYER_MAX_AOE_EFFECTS; i++) {
        if (!_aoeEffects[i].active) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        float minTime = _aoeEffects[0].remainingTime;
        slot = 0;
        for (int i = 1; i < PLAYER_MAX_AOE_EFFECTS; i++) {
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

void GEPlayer::drawAoeIndicator(GEWindow& window, const GECamera& camera) const {
    if (!SHOW_PLAYER_AOE_INDICATOR) {
        return;
    }

    if (_aoeCooldownTimer > 0.0f) {
        return;
    }

    drawCircle(window, camera, getCenterX(), getCenterY(), _aoeRadius, 0, 200, 255);
}

void GEPlayer::drawAoeEffects(GEWindow& window, const GECamera& camera) const {
    for (int i = 0; i < PLAYER_MAX_AOE_EFFECTS; i++) {
        if (!_aoeEffects[i].active) continue;
        drawCircle(window, camera, _aoeEffects[i].centerX, _aoeEffects[i].centerY, _aoeEffects[i].radius, _aoeEffects[i].colorR, _aoeEffects[i].colorG, _aoeEffects[i].colorB);
    }
}

void GEPlayer::drawCircle(GEWindow& window, const GECamera& camera, float centerX, float centerY, float radius, unsigned char r, unsigned char g, unsigned char b) const {
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

    for (int dx = -radiusInt; dx <= radiusInt; ++dx) {
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

void GEPlayer::draw(GEWindow& window, const GECamera& camera) {
    GECollisible::draw(window, camera);
    drawAoeIndicator(window, camera);
    drawAoeEffects(window, camera);
}
