#include "GEMapsManager.h"

constexpr auto TILES_COUNT = 26;

GEMapsManager::GEMapsManager() {}

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

void GEMapsManager::draw(Window& window, GECamera& camera) {
    if (!_saveData) return;

    int layers = _saveData->getLayerCount();
    int mapWidth = _saveData->getMapColCount();
    int mapHeight = _saveData->getMapRowCount();
    int tileWidth = _saveData->getTileWidth();
    int tileHeight = _saveData->getTileHeight();
    int cameraOffsetX = _saveData->getCameraOffsetX();
    int cameraOffsetY = _saveData->getCameraOffsetY();

    int winWidth = _saveData->getScreenWidth();
    int winHeight = _saveData->getScreenHeight();

    for (int layer = 0;layer < layers;layer++) {
        for (int rowNumber = 0;rowNumber < mapHeight;rowNumber++) {
            for (int colNumber = 0;colNumber < mapWidth;colNumber++) {
                int tileID = _saveData->getTileID(layer, rowNumber, colNumber);
                GETile* tile = getTile(tileID);
				if (!tile) continue;

				const float tileCenterX = (colNumber + 0.5f) * tileWidth;
				const float tileCenterY = (rowNumber + 0.5f) * tileHeight;
				tile->setCenter(tileCenterX, tileCenterY);
				tile->draw(window, camera);
            }
        }
    }
}
