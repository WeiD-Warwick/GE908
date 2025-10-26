//
// Created by W.D. on 26/10/25
//
#include "GESaveData.h"

bool GESaveData::_allocateLayers() {
	return false;
}

void GESaveData::_releaseLayers() {
}

GESaveData::GESaveData() : _mapWidth(0), _mapHeight(0), _tileWidth(0), _tileHeight(0), _layers(nullptr), _layerCount(0) {
	_allocateLayers();
}

GESaveData::~GESaveData() {
	_releaseLayers();
}

bool GESaveData::saveGame(const char* filename) {
	return false;
}

bool GESaveData::loadGame(const char* filename) {
	return false;
}

int GESaveData::getTileID(int layer, int x, int y) const {
	return 0;
}
