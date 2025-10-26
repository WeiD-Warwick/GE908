//
// Created by W.D. on 26/10/25
//

#pragma once
class GESaveData {
private:
	int _mapWidth;
	int _mapHeight;
	int _tileWidth;
	int _tileHeight;

	int*** _layers;
	int _layerCount;

	bool _allocateLayers();
	void _releaseLayers();


public:
	GESaveData();
	~GESaveData();

	bool saveGame(const char* filename);
	bool loadGame(const char* filename);

	int getTilesWide() const { return _mapWidth; }
	int getTilesHigh() const { return _mapHeight; }
	int getTileWidth() const { return _tileWidth; }
	int getTileHeight() const { return _tileHeight; }

	int getTileID(int layer, int x, int y) const;
};

