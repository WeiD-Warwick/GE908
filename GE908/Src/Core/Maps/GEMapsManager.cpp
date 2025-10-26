#include "GEMapsManager.h"
#define TILESCOUNT 24

GEMapsManager::GEMapsManager() {
}

GEMapsManager::~GEMapsManager() {
	if (_tileImages) {
		for (int i = 0; i < TILESCOUNT; i++) {
			delete _tileImages[i];
		}
		delete[] _tileImages;
	}
}

bool GEMapsManager::loadTileResources(const std::string& folderPath) {
	_tileImages = new Image * [TILESCOUNT];

	for (int i = 0; i <= 23; i++) {
		_tileImages[i] = new Image();
		std::string filePath = folderPath + std::to_string(i) + ".png";
		if (!_tileImages[i]->load(filePath)) {
			GELog::shared().error("Load tile: " + std::to_string(i) + " failed.");
			return false;
		}
	}

	GELog::shared().info("Load tiles success.");
	return true;
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

Image* GEMapsManager::getTileImage(int tileID) const {
	if (tileID < 0 || tileID >= TILESCOUNT) return nullptr;
	return _tileImages[tileID];
}

