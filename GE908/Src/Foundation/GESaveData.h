#pragma once
#include <string>

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

	void setCameraOffset(int x, int y) { _cameraOffsetX = x; _cameraOffsetY = y; }
	float getCameraOffsetX() { return _cameraOffsetX; }
	float getCameraOffsetY() { return _cameraOffsetY; }

	void setWindowSize(int width, int height) { _windowWidth = width; _windowHeight = height; }
	int getScreenWidth() { return _windowWidth; }
	int getScreenHeight() { return _windowHeight;  }

	int getLayerCount() const { return _layerCount; }

	int getTileID(int layer, int row, int col) const;

};

