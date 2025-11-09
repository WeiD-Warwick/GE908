#include "GEMapsManager.h"
#include <cmath>

GEMapsManager::GEMapsManager() = default;

GEMapsManager::~GEMapsManager() {
	if (_tiles) {
		for (int i = 0;i < Map::TILES_COUNT;i++) {
			delete _tiles[i];
		}
		delete[] _tiles;
	}
}

void GEMapsManager::load(GESaveData* saveData) {
	_saveData = saveData;
}

void GEMapsManager::loadTileResources(const std::string& folderPath) {
	_tiles = new GETile * [Map::TILES_COUNT];

	for (int i = 0;i < Map::TILES_COUNT - 1;i++) {
		std::string filePath = folderPath + std::to_string(i) + ".png";

		if (i <= 13 || i == 23) {
			_tiles[i] = new GETile(filePath, GECollisionType::None);
		}
		else if (i == 24) {
			_tiles[i] = new GETile(filePath, GECollisionType::Tile_Fire);
		}
		else {
			_tiles[i] = new GETile(filePath, GECollisionType::Tile_Water);
		}
	}
}

GETile* GEMapsManager::getTile(int tileID) const {
	if (tileID < 0 || tileID >= Map::TILES_COUNT) return nullptr;
	return _tiles[tileID];
}

void GEMapsManager::draw(Window& window, const GECamera& camera) const {
    if (!_saveData) return;

	const int tileWidth = _saveData->getTileWidth();
	const int tileHeight = _saveData->getTileHeight();

	if (tileWidth <= 0 || tileHeight <= 0) return;

	const float cameraX = camera.getX();
	const float cameraY = camera.getY();
	const float cameraRight = cameraX + static_cast<float>(camera.getWidth());
	const float cameraBottom = cameraY + static_cast<float>(camera.getHeight());

	int minCol = static_cast<int>(std::floor(cameraX / tileWidth));
	int maxCol = static_cast<int>(std::floor((cameraRight - 1.0f) / tileWidth));
	int minRow = static_cast<int>(std::floor(cameraY / tileHeight));
	int maxRow = static_cast<int>(std::floor((cameraBottom - 1.0f) / tileHeight));

	// Expand the range slightly to avoid visible gaps at the edges.
	minCol -= 1;
	minRow -= 1;
	maxCol += 1;
	maxRow += 1;

	for (int rowNumber = minRow; rowNumber <= maxRow; rowNumber++) {
		for (int colNumber = minCol; colNumber <= maxCol; colNumber++) {
			int tileID = _saveData->getTileID(rowNumber, colNumber);
			GETile* tile = getTile(tileID);
			if (!tile) continue;
			const float tileCenterX = (colNumber + 0.5f) * tileWidth;
			const float tileCenterY = (rowNumber + 0.5f) * tileHeight;
			tile->setCenter(tileCenterX, tileCenterY);
			tile->draw(window, camera);
		}
	}
}
