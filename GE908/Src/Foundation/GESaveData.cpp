//
// Created by W.D. on 26/10/25
//

#include "GESaveData.h"
#include "GELog.h"
#include <fstream>
#include <sstream>

bool GESaveData::_allocateLayers(int layerCount) {

    if (_mapColCount <= 0 || _mapRowCount <= 0 || layerCount <= 0) return false;

    _layers = new int** [layerCount];
    for (int layer = 0; layer < layerCount; ++layer) {
        _layers[layer] = new int* [_mapRowCount];
        for (int y = 0; y < _mapRowCount; ++y) {
            _layers[layer][y] = new int[_mapColCount];
            for (int x = 0; x < _mapColCount; ++x)
                _layers[layer][y][x] = 0;
        }
    }

    _layerCount = layerCount;
    GELog::shared().info("Allocated new mapLayers: " + std::to_string(_layerCount)
                        + ", width: " + std::to_string(_mapColCount) 
                        + ", height: " + std::to_string(_mapRowCount));

    return true;
}

void GESaveData::_releaseLayers() {
    if (!_layers) return;

    for (int layer = 0; layer < _layerCount; ++layer) {
        for (int y = 0; y < _mapRowCount; ++y)
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

	if (key == "tileswide") _mapColCount = value;
	else if (key == "tileshigh") _mapRowCount = value;
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
                    newLayers[l] = new int* [_mapRowCount];
                    for (int y = 0; y < _mapRowCount; ++y) {
                        newLayers[l][y] = new int[_mapColCount];
                        for (int x = 0; x < _mapColCount; ++x)
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

            while (std::getline(ss, cell, ',') && x < _mapColCount) {
                _layers[currentLayer][currentRow][x] = std::stoi(cell);
                ++x;
            }

            currentRow++;
            if (currentRow >= _mapRowCount) startReadingLayer = false;
        }
    }

    file.close();
    return true;
}

int GESaveData::getTileID(int layer, int row, int col) const {
    if (!_layers || layer < 0 || layer >= _layerCount) return 0;
    if (row < 0 || col < 0 || col >= _mapColCount || row >= _mapRowCount) return 0;
    return _layers[layer][row][col];
}
