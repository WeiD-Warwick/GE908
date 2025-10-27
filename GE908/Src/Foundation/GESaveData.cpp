//
// Created by W.D. on 26/10/25
//

#include "GESaveData.h"
#include "GELog.h"
#include <fstream>
#include <sstream>

bool GESaveData::_allocateLayers(int layerCount) {

    if (_mapWidth <= 0 || _mapHeight <= 0 || layerCount <= 0) return false;

    _layers = new int** [layerCount];
    for (int layer = 0; layer < layerCount; ++layer) {
        _layers[layer] = new int* [_mapHeight];
        for (int y = 0; y < _mapHeight; ++y) {
            _layers[layer][y] = new int[_mapWidth];
            for (int x = 0; x < _mapWidth; ++x)
                _layers[layer][y][x] = 0;
        }
    }

    _layerCount = layerCount;
    GELog::shared().info("Allocated new mapLayers: " + std::to_string(_layerCount)
                        + ", width: " + std::to_string(_mapWidth) 
                        + ", height: " + std::to_string(_mapHeight));

    return true;
}

void GESaveData::_releaseLayers() {
    if (!_layers) return;

    for (int layer = 0; layer < _layerCount; ++layer) {
        for (int y = 0; y < _mapHeight; ++y)
            delete[] _layers[layer][y];
        delete[] _layers[layer];
    }
    delete[] _layers;
    _layers = nullptr;
    _layerCount = 0;

	GELog::shared().info("Released all map layers.");
}

bool GESaveData::_parseKeywordLine(const std::string& line) {
	if (line.empty()) return false;

	std::istringstream iss(line);
	std::string key;

	int value = 0;

	if (!(iss >> key >> value)) return false;

	if (key == "tileswide") _mapWidth = value;
	else if (key == "tileshigh") _mapHeight = value;
	else if (key == "tilewidth") _tileWidth = value;
	else if (key == "tileheight") _tileHeight = value;
	return true;
}

GESaveData::GESaveData() {}

GESaveData::~GESaveData() {
	_releaseLayers();
}

bool GESaveData::saveGame(const std::string& filename) {
	return false;
}

bool GESaveData::loadGame(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        GELog::shared().error("Failed to open save file: " + std::string(filename));
        return false;
    }
    
    _releaseLayers();
    _layerCount = 0;

	// begin parsing file
    std::string line;
    int currentLayer = -1;
    int currentRow = 0;
    bool startReadingLayer = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

		// read matadata line
        if (!startReadingLayer && line.find("layer") != 0) {
            _parseKeywordLine(line);
            continue;
        }

		// read layer number
        if (line.find("layer") == 0) {
            currentRow = 0;
            startReadingLayer = true;

			// parse layer index
            std::string tmp;
            std::istringstream iss(line);
            iss >> tmp >> currentLayer;

            if (!_layers) {
                _allocateLayers(currentLayer + 1);
            } else if (currentLayer >= _layerCount) {
				// need to expand layers array
                int oldCount = _layerCount;
                int*** newLayers = new int** [currentLayer + 1];
                for (int l = 0; l < oldCount; ++l) newLayers[l] = _layers[l];
                for (int l = oldCount; l <= currentLayer; ++l) {
                    newLayers[l] = new int* [_mapHeight];
                    for (int y = 0; y < _mapHeight; ++y) {
                        newLayers[l][y] = new int[_mapWidth];
                        for (int x = 0; x < _mapWidth; ++x)
                            newLayers[l][y][x] = 0;
                    }
                }
                delete[] _layers;
                _layers = newLayers;
                _layerCount = currentLayer + 1;
            }
            continue;
        }

		// read layer data
        if (startReadingLayer && currentLayer >= 0 && currentLayer < _layerCount) {
            std::stringstream ss(line);
            std::string cell;
            int x = 0;

            while (std::getline(ss, cell, ',') && x < _mapWidth) {
                _layers[currentLayer][currentRow][x] = std::stoi(cell);
                ++x;
            }

            currentRow++;
            if (currentRow >= _mapHeight) startReadingLayer = false;
        }
    }

    //for (int l = 0; l < _layerCount; l++) {
    //    for (int y = 0; y < _mapHeight; y++) {
    //        for (int x = 0; x < _mapWidth; x++) {
    //            std::cout << _layers[l][y][x] << std::endl;
    //        }
    //    }
    //}

    file.close();
    return true;
}

int GESaveData::getTileID(int layer, int row, int col) const {
    if (!_layers || layer < 0 || layer >= _layerCount) return 0;
    if (row < 0 || col < 0 || col >= _mapWidth || row >= _mapHeight) return 0;
    return _layers[layer][row][col];
}
