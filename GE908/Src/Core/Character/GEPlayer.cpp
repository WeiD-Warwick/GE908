#include "GEPlayer.h"
#include "BaseCharacter.h"
#include "GEEnemyManager.h"
#include "../../Foundation/GESaveData.h"

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

bool GEPlayer::collidesWithWater(int newX, int newY) const {
    if (!_saveData) return false;

    int tileWidth = _saveData->getTileWidth();
    int tileHeight = _saveData->getTileHeight();
    if (tileWidth == 0 || tileHeight == 0) return false;

    int mapCols = _saveData->getMapColCount();
    int mapRows = _saveData->getMapRowCount();
    int layerCount = _saveData->getLayerCount();

    if (mapCols == 0 || mapRows == 0) return false;

    int left = newX;
    int right = newX + _width - 1;
    int top = newY;
    int bottom = newY + _height - 1;

    int startCol = clamp(left / tileWidth, 0, mapCols - 1);
    int endCol = clamp(right / tileWidth, 0, mapCols - 1);
    int startRow = clamp(top / tileHeight, 0, mapRows - 1);
    int endRow = clamp(bottom / tileHeight, 0, mapRows - 1);

    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            for (int layer = 0; layer < layerCount; ++layer) {
                int tileID = _saveData->getTileID(layer, row, col);
                if (isWaterTile(tileID)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool GEPlayer::collidesWithEnemies(int newX, int newY, const GEEnemyManager& enemyManager) const {
    int playerCenterX = newX + _width / 2;
    int playerCenterY = newY + _height / 2;
    int currentCenterX = _originX + _width / 2;
    int currentCenterY = _originY + _height / 2;
    int playerRadius = getCollisionRadius();

    int enemyCount = enemyManager.getEnemyCount();
    for (int i = 0; i < enemyCount; ++i) {
        GEEnemy* enemy = enemyManager.getEnemyAt(i);
        if (!enemy) continue;

        int enemyCenterX = enemy->getX() + enemy->getWidth() / 2;
        int enemyCenterY = enemy->getY() + enemy->getHeight() / 2;
        int combinedRadius = playerRadius + enemy->getCollisionRadius();
        int combinedRadiusSq = combinedRadius * combinedRadius;

        int dx = playerCenterX - enemyCenterX;
        int dy = playerCenterY - enemyCenterY;
        int newDistanceSq = dx * dx + dy * dy;

        if (newDistanceSq < combinedRadiusSq) {
            int currentDx = currentCenterX - enemyCenterX;
            int currentDy = currentCenterY - enemyCenterY;
            int currentDistanceSq = currentDx * currentDx + currentDy * currentDy;

            if (currentDistanceSq >= combinedRadiusSq || newDistanceSq <= currentDistanceSq) {
                return true;
            }
        }
    }

    return false;
}

void GEPlayer::update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight, const GEEnemyManager& enemyManager) {
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
        if (!collidesWithWater(proposedX, _originY) && !collidesWithEnemies(proposedX, _originY, enemyManager)) {
            _originX = proposedX;
        }
    }

    if (deltaY != 0) {
        int proposedY = clamp(_originY + deltaY, minOriginY, maxOriginY);
        if (!collidesWithWater(_originX, proposedY) && !collidesWithEnemies(_originX, proposedY, enemyManager)) {
            _originY = proposedY;
        }
    }

    _originX = clamp(_originX, minOriginX, maxOriginX);
    _originY = clamp(_originY, minOriginY, maxOriginY);
}