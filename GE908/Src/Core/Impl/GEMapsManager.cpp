#include "GEMapsManager.h"

constexpr auto TILES_COUNT = 26;

GEMapsManager::GEMapsManager() = default;

GEMapsManager::~GEMapsManager() {
	if (_tiles) {
		for (int i = 0;i < TILES_COUNT;i++) {
			delete _tiles[i];
		}
		delete[] _tiles;
	}
}

void GEMapsManager::load(const std::string& tilesFolderPath, const std::string& saveDatafilePath) {
	loadTileResources(tilesFolderPath);
	loadSaveData(saveDatafilePath);
}

void GEMapsManager::loadTileResources(const std::string& folderPath) {
	_tiles = new GETile * [TILES_COUNT];

	for (int i = 0;i < TILES_COUNT - 1;i++) {
		std::string filePath = folderPath + std::to_string(i) + ".png";

		if (i <= 13 || i == 23) {
			_tiles[i] = new GETile(filePath, GECollisionType::None);
		}
		else if (i == 24) {
			_tiles[i] = new GETile(filePath, GECollisionType::Fire);
		}
		else {
			_tiles[i] = new GETile(filePath, GECollisionType::Water);
		}
	}
}

void GEMapsManager::loadSaveData(const std::string& filePath) {
	if (_saveData) {
		delete _saveData;
		_saveData = nullptr;
	}
	_saveData = new GESaveData();

	if (!_saveData->loadGame(filePath)) {
		delete _saveData;
		_saveData = nullptr;
	}
}

GETile* GEMapsManager::getTile(int tileID) const {
	if (tileID < 0 || tileID >= TILES_COUNT) return nullptr;
	return _tiles[tileID];
}

void GEMapsManager::draw(Window& window, const GECamera& camera) const {
    if (!_saveData) return;

	const int layers = _saveData->getLayerCount();
	const int mapWidth = _saveData->getMapColCount();
	const int mapHeight = _saveData->getMapRowCount();
	const int tileWidth = _saveData->getTileWidth();
	const int tileHeight = _saveData->getTileHeight();

	if (mapWidth <= 0 || mapHeight <= 0 || tileWidth <= 0 || tileHeight <= 0) return;

    int cameraOffsetX = _saveData->getCameraOffsetX();
    int cameraOffsetY = _saveData->getCameraOffsetY();

    int winWidth = _saveData->getScreenWidth();
    int winHeight = _saveData->getScreenHeight();

	const float cameraX = camera.getX();
	const float cameraY = camera.getY();
	const float cameraRight = cameraX + static_cast<float>(camera.getWidth());
	const float cameraBottom = cameraY + static_cast<float>(camera.getHeight());

	int minCol = static_cast<int>(std::floor(cameraX / tileWidth));
	int maxCol = static_cast<int>(std::floor((cameraRight - 1.0f) / tileWidth));
	int minRow = static_cast<int>(std::floor(cameraY / tileHeight));
	int maxRow = static_cast<int>(std::floor((cameraBottom - 1.0f) / tileHeight));

	if (maxCol < 0 || maxRow < 0 || minCol >= mapWidth || minRow >= mapHeight)
		return;

	if (minCol < 0) minCol = 0;
	if (minRow < 0) minRow = 0;
	if (maxCol >= mapWidth) maxCol = mapWidth - 1;
	if (maxRow >= mapHeight) maxRow = mapHeight - 1;

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
