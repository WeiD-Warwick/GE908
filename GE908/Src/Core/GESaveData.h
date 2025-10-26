//
// Created by W.D. on 26/10/25
//

#include <iostream>
#pragma once
class GESaveData {
private:
	int _mapWidth = 0;
	int _mapHeight = 0;
	int _tileWidth = 0;
	int _tileHeight = 0;

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

	int getMapWidth() const { return _mapWidth; }
	int getMapHeight() const { return _mapHeight; }
	int getTileWidth() const { return _tileWidth; }
	int getTileHeight() const { return _tileHeight; }
	int getLayerCount() const { return _layerCount; }

	int getTileID(int layer, int x, int y) const;

};

