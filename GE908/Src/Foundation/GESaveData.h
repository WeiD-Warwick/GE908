//
// Created by W.D. on 26/10/25
//

#include <iostream>
#pragma once
class GESaveData {
private:
	int _mapColCount = 0;
	int _mapRowCount = 0;
	int _tileWidth = 0;
	int _tileHeight = 0;

	int _cameraOffsetX = 0;
	int _cameraOffsetY = 0;

	int _windowWidth = 0;
	int _windowHeight = 0;

	int*** _layers = nullptr;
	int _layerCount = 0;

	bool _allocateLayers(int layerCount);
	void _releaseLayers();
	bool _parseKeywordLine(const std::string& line);

public:
	GESaveData();
	~GESaveData();

	bool saveGame(const std::string& filename);
	bool loadGame(const std::string& filename);

	int getMapColCount() const { return _mapColCount; }
	int getMapRowCount() const { return _mapRowCount; }

	int getTileWidth() const { return _tileWidth; }
	int getTileHeight() const { return _tileHeight; }

	int getMapTotalWidth() const { return _mapColCount * _tileWidth; }
	int getMapTotalHeight() const { return _mapRowCount * _tileHeight; }

	void setCameraOffsetX(int x) { _cameraOffsetX = x; }
	void setCameraOffsetY(int y) { _cameraOffsetY = y; }
	int getCameraOffsetX() { return _cameraOffsetX; }
	int getCameraOffsetY() { return _cameraOffsetY; }

	void setWindowWidth(int width) { _windowWidth = width; }
	void setWindowHeight(int height) { _windowHeight = height; }
	int getWindowWidth() { return _windowWidth; }
	int getWindowHeight() { return _windowHeight;  }

	int getLayerCount() const { return _layerCount; }

	int getTileID(int layer, int row, int col) const;

};

