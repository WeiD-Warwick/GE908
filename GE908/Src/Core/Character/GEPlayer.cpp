#include "GEPlayer.h"
#include "BaseCharacter.h"
#include "GEEnemyManager.h"
#include "../../Foundation/GESaveData.h"
#include "../Maps/GEMapsManager.h"
#include "../Skill/GEProjectileManager.h"

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


void GEPlayer::update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight, const GEMapsManager& mapsManager, const GEEnemyManager& enemyManager) {
    if (!_saveData) return;

    float moveDelta = _speed * deltaTime;
    int moveAmount = static_cast<int>(moveDelta > 1.0f ? moveDelta : 1.0f);

    int deltaX = 0;
    int deltaY = 0;

    if (moveUp) deltaY -= moveAmount;
    if (moveDown) deltaY += moveAmount;
    if (moveLeft) deltaX -= moveAmount;
    if (moveRight) deltaX += moveAmount;

    int cameraWidth = _saveData->getScreenWidth();
    int cameraHeight = _saveData->getScreenHeight();
    int mapTotalWidth = _mapWidth;
    int mapTotalHeight = _mapHeight;

    int minCenterX, maxCenterX;
    if (mapTotalWidth > cameraWidth) {
        minCenterX = cameraWidth / 2;
        maxCenterX = mapTotalWidth - cameraWidth / 2;
    }
    else {
        minCenterX = maxCenterX = mapTotalWidth / 2;
    }

    int minCenterY, maxCenterY;
    if (mapTotalHeight > cameraHeight) {
        minCenterY = cameraHeight / 2;
        maxCenterY = mapTotalHeight - cameraHeight / 2;
    }
    else {
        minCenterY = maxCenterY = mapTotalHeight / 2;
    }

    int playerHalfWidth = _width / 2;
    int playerHalfHeight = _height / 2;
    int minOriginX = minCenterX - playerHalfWidth;
    int maxOriginX = maxCenterX - playerHalfWidth;
    int minOriginY = minCenterY - playerHalfHeight;
    int maxOriginY = maxCenterY - playerHalfHeight;

    if (deltaX != 0) {
        int proposedX = clamp(_originX + deltaX, minOriginX, maxOriginX);
        if (!collidesWithWater(proposedX, _originY, mapsManager) && !collidesWithEnemies(proposedX, _originY, enemyManager)) {
            _originX = proposedX;
        }
    }

    if (deltaY != 0) {
        int proposedY = clamp(_originY + deltaY, minOriginY, maxOriginY);
        if (!collidesWithWater(_originX, proposedY, mapsManager) && !collidesWithEnemies(_originX, proposedY, enemyManager)) {
            _originY = proposedY;
        }
    }

    _originX = clamp(_originX, minOriginX, maxOriginX);
    _originY = clamp(_originY, minOriginY, maxOriginY);
}


void GEPlayer::updateAttack(float deltaTime, const GEEnemyManager& enemyManager, GEProjectileManager& projectileManager) {
    _attackTimer += deltaTime;

    if (_attackTimer < _attackInterval) return;

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

