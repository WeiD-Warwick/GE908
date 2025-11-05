#include "GEMapsManager.h"
#include "../../Foundation/GELog.h"
#define TILESCOUNT 24

GEMapsManager::GEMapsManager() {
}

GEMapsManager::~GEMapsManager() {
	if (_tiles) {
		for (int i = 0; i < TILESCOUNT; i++) {
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
	_tiles = new GETile * [TILESCOUNT];

	for (int i = 0; i <= 23; i++) {
		std::string filePath = folderPath + std::to_string(i) + ".png";

		if (i <= 13 || i >= 23) { 
			_tiles[i] = new GETile(filePath, None);
		} else {
			_tiles[i] = new GETile(filePath, Water);
		}
	}

	GELog::shared().info("Load tiles success.");
}

void GEMapsManager::loadSaveData(const std::string& filePath) {
	if (_saveData) {
		delete _saveData;
		_saveData = nullptr;
	}
	_saveData = new GESaveData();

	if (_saveData->loadGame(filePath)) {
		GELog::shared().info("Load Save Data: " + filePath + " Success");
	} else {
		delete _saveData;
		_saveData = nullptr;
		GELog::shared().error("Load Save Data: " + filePath + " Failed");
	}
}

GETile* GEMapsManager::getTile(int tileID) const {
	if (tileID < 0 || tileID >= TILESCOUNT) return nullptr;
	return _tiles[tileID];
}

void GEMapsManager::draw(GEWindow& window, GECamera& camera) {
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

    for (int layer = 0; layer < layers; layer++) {
        for (int rowNumber = 0; rowNumber < mapHeight; rowNumber++) {
            for (int colNumber = 0; colNumber < mapWidth; colNumber++) {
                int tileID = _saveData->getTileID(layer, rowNumber, colNumber);
                GETile* tile = getTile(tileID);
				if (!tile) continue;

				tile->setCenter(colNumber * tileWidth, rowNumber * tileHeight);
				tile->draw(window, camera);
            }
        }
    }
}
