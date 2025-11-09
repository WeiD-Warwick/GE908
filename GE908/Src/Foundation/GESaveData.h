#pragma once
#include <string>
#include <fstream>
#include <sstream>

class GESaveData {
private:
    int _mapColCount = 0;
    int _mapRowCount = 0;
    int _tileWidth = 0;
    int _tileHeight = 0;

    float _cameraOffsetX = 0;
    float _cameraOffsetY = 0;

    int _windowWidth = 854;
    int _windowHeight = 480;

    // Changed to 2D array (only keep single layer)
    int** _layer = nullptr;

    // Allocate memory for single layer
    bool _allocateLayer() {
        if (_mapColCount <= 0 || _mapRowCount <= 0) return false;

        _layer = new int* [_mapRowCount];
        for (int y = 0; y < _mapRowCount; ++y) {
            _layer[y] = new int[_mapColCount];
            for (int x = 0; x < _mapColCount; ++x)
                _layer[y][x] = 0;
        }
        return true;
    }

    // Release memory for single layer
    void _releaseLayer() {
        if (!_layer) return;

        for (int y = 0; y < _mapRowCount; ++y)
            delete[] _layer[y];
        delete[] _layer;
        _layer = nullptr;
    }

    bool _parseKeywordLine(const std::string& line) {
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

public:
    GESaveData() = default;
    ~GESaveData() {
        _releaseLayer(); // Release single layer
    }

    int getMapColCount() const { return _mapColCount; }
    int getMapRowCount() const { return _mapRowCount; }

    int getTileWidth() const { return _tileWidth; }
    int getTileHeight() const { return _tileHeight; }

    int getMapTotalWidth() const { return _mapColCount * _tileWidth; }
    int getMapTotalHeight() const { return _mapRowCount * _tileHeight; }

    void setCameraOffset(int x, int y) { _cameraOffsetX = x; _cameraOffsetY = y; }
    float getCameraOffsetX() { return _cameraOffsetX; }
    float getCameraOffsetY() { return _cameraOffsetY; }

    void setWindowSize(int width, int height) { _windowWidth = width; _windowHeight = height; }
    int getScreenWidth() const { return _windowWidth; }
    int getScreenHeight() const { return _windowHeight; }

    // Since there's only one layer, return fixed value 1 or remove this interface
    int getLayerCount() const { return 1; }

    bool loadGame(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        _releaseLayer(); // Release old data before loading

        std::string line;
        int currentRow = 0;
        bool startReadingLayer = false;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Read metadata (when not starting to read layer yet)
            if (!startReadingLayer && line.find("layer") != 0) {
                _parseKeywordLine(line);
                continue;
            }

            // Detect layer marker (only process layer 0)
            if (line.find("layer 0") == 0) {
                currentRow = 0;
                startReadingLayer = true;
                // Allocate layer memory (metadata has been parsed, can get row and column counts)
                if (!_allocateLayer()) {
                    return false; // Memory allocation failed
                }
                continue;
            }

            // Read layer data (only process single layer)
            if (startReadingLayer) {
                std::stringstream ss(line);
                std::string cell;
                int x = 0;

                while (std::getline(ss, cell, ',') && x < _mapColCount) {
                    try {
                        _layer[currentRow][x] = std::stoi(cell);
                    }
                    catch (...) {
                        // Set to 0 by default if parsing fails
                        _layer[currentRow][x] = 0;
                    }
                    ++x;
                }

                currentRow++;
                // Stop reading layer when reaching map height
                if (currentRow >= _mapRowCount) {
                    startReadingLayer = false;
                }
            }
        }

        file.close();
        return true;
    }

    // Remove layer parameter, directly access 2D array
    int getTileID(int row, int col) const {
        if (!_layer) return 0;
        if (row < 0 || row >= _mapRowCount || col < 0 || col >= _mapColCount) return 0;
        return _layer[row][col];
    }
};