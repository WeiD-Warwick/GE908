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

    moveUpdate(deltaTime, dirX, dirY);

    autoAttack(deltaTime);
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
    float len = sqrtf(dirX * dirX + dirY * dirY);
    if (len == 0.0f) {
        return;
    }
    dirX /= len;
    dirY /= len;

    _projectileManager->addProjectile(FromPlayer, playerCenterX, playerCenterY, dirX, dirY, 100.0f, 200);
}
