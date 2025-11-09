#include "GESaveData.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>

GESaveData::GESaveData() = default;
GESaveData::~GESaveData() { clearChunks(); }

bool GESaveData::_parseKeywordLine(const std::string& line, MapChunk& chunk) {
    if (line.empty()) return false;
    std::istringstream iss(line);
    std::string key;
    if (!(iss >> key)) return false;

    if (key == "mapmode") {
        std::string mode;
        if (!(iss >> mode)) return false;
        _infiniteMap = (mode == "infinite");
        return true;
    }

    if (key == "seed") {
        uint32_t value = 0u;
        if (!(iss >> value)) return false;
        _randomSeed = value;
        _hasRandomSeed = true;
        return true;
    }

    int value = 0;
    if (!(iss >> value)) return false;

    if (key == "tileswide") chunk.columns = value;
    else if (key == "tileshigh") chunk.rows = value;
    else if (key == "tilewidth") _tileWidth = value;
    else if (key == "tileheight") _tileHeight = value;
    return true;
}

void GESaveData::clearChunks() {
    ChunkNode* node = _chunksHead;
    while (node) {
        ChunkNode* next = node->next;
        delete node;
        node = next;
    }
    _chunksHead = nullptr;
    _baseChunk.clear();
    _chunkColumns = 0;
    _chunkRows = 0;
    _infiniteMap = false;
    _activeChunk = { 0,0 };
    _randomSeed = 0u;
    _hasRandomSeed = false;
}

ChunkNode* GESaveData::findChunkNode(const ChunkCoord& coord) {
    ChunkNode* node = _chunksHead;
    while (node) {
        if (node->coord == coord) return node;
        node = node->next;
    }
    return nullptr;
}

const ChunkNode* GESaveData::findChunkNode(const ChunkCoord& coord) const {
    const ChunkNode* node = _chunksHead;
    while (node) {
        if (node->coord == coord) return node;
        node = node->next;
    }
    return nullptr;
}

int GESaveData::floorDiv(int value, int divisor) {
    int q = value / divisor;
    int r = value % divisor;
    if ((r != 0) && ((r < 0) != (divisor < 0))) --q;
    return q;
}

int GESaveData::positiveMod(int value, int divisor) {
    int mod = value % divisor;
    if (mod < 0) mod += std::abs(divisor);
    return mod;
}

bool GESaveData::worldToChunkIndices(int tileRow, int tileCol, ChunkCoord& coord, int& localRow, int& localCol) const {
    if (_chunkColumns <= 0 || _chunkRows <= 0) return false;
    coord.x = floorDiv(tileCol, _chunkColumns);
    coord.y = floorDiv(tileRow, _chunkRows);
    localCol = positiveMod(tileCol, _chunkColumns);
    localRow = positiveMod(tileRow, _chunkRows);
    return true;
}

uint32_t GESaveData::hashCoordinates(int worldRow, int worldCol) const {
    const uint32_t prime = 101;
    uint32_t hash = _randomSeed;
    hash = hash * prime + worldRow;
    hash = hash * prime + worldCol;
    return hash;
}

int GESaveData::sampleTileForWorld(int worldRow, int worldCol) const {
    if (!_baseChunk.isValid()) return 0;
    if (!_hasRandomSeed) {
        const int srcRow = positiveMod(worldRow, _chunkRows);
        const int srcCol = positiveMod(worldCol, _chunkColumns);
        return _baseChunk.getTileID(srcRow, srcCol);
    }
    const uint32_t hashed = hashCoordinates(worldRow, worldCol);
    const int srcRow = static_cast<int>(hashed % static_cast<uint32_t>(_chunkRows));
    const int srcCol = static_cast<int>((hashed / static_cast<uint32_t>(_chunkRows)) % static_cast<uint32_t>(_chunkColumns));
    return _baseChunk.getTileID(srcRow, srcCol);
}

void GESaveData::generateChunk(MapChunk& chunk, const ChunkCoord& coord) {
    if (!_baseChunk.isValid()) return;
    if (coord.x == 0 && coord.y == 0) { chunk = _baseChunk; return; }
    if (!chunk.allocate(_chunkColumns, _chunkRows)) return;

    for (int row = 0; row < _chunkRows; ++row)
        for (int col = 0; col < _chunkColumns; ++col) {
            int worldRow = coord.y * _chunkRows + row;
            int worldCol = coord.x * _chunkColumns + col;
            int tile = sampleTileForWorld(worldRow, worldCol);
            chunk.tiles[row * _chunkColumns + col] = tile;
        }
}

ChunkNode* GESaveData::ensureChunkNode(const ChunkCoord& coord) {
    ChunkNode* node = findChunkNode(coord);
    if (node) return node;
    if (!_infiniteMap && (coord.x != 0 || coord.y != 0)) return nullptr;
    node = new ChunkNode();
    node->coord = coord;
    node->next = _chunksHead;
    _chunksHead = node;
    if (_infiniteMap) generateChunk(node->chunk, coord);
    else if (_baseChunk.isValid()) node->chunk = _baseChunk;
    return node;
}

void GESaveData::setCameraOffset(int x, int y) {
    _cameraOffsetX = static_cast<float>(x);
    _cameraOffsetY = static_cast<float>(y);
}

float GESaveData::getCameraOffsetX() { return _cameraOffsetX; }
float GESaveData::getCameraOffsetY() { return _cameraOffsetY; }

void GESaveData::setWindowSize(int w, int h) { _windowWidth = w; _windowHeight = h; }
int GESaveData::getScreenWidth() const { return _windowWidth; }
int GESaveData::getScreenHeight() const { return _windowHeight; }

void GESaveData::setActiveChunk(int chunkX, int chunkY) {
    if (!_infiniteMap) { _activeChunk = { 0,0 }; ensureChunkNode(_activeChunk); return; }
    _activeChunk.x = chunkX;
    _activeChunk.y = chunkY;
    ensureChunkNode(_activeChunk);
}

void GESaveData::updateActiveChunkFromWorldPosition(float worldX, float worldY) {
    if (!_infiniteMap) return;
    if (_tileWidth <= 0 || _tileHeight <= 0) return;
    int tileCol = static_cast<int>(std::floor(worldX / _tileWidth));
    int tileRow = static_cast<int>(std::floor(worldY / _tileHeight));
    ChunkCoord coord; int localRow = 0, localCol = 0;
    if (!worldToChunkIndices(tileRow, tileCol, coord, localRow, localCol)) return;
    setActiveChunk(coord.x, coord.y);
}

const MapChunk* GESaveData::getActiveChunk() const {
    const ChunkNode* node = findChunkNode(_activeChunk);
    return node ? &node->chunk : nullptr;
}

MapChunk* GESaveData::getActiveChunk() {
    ChunkNode* node = findChunkNode(_activeChunk);
    if (!node) {
        if (!_infiniteMap && _chunksHead) return &_chunksHead->chunk;
        return nullptr;
    }
    return &node->chunk;
}

int GESaveData::getActiveChunkColumnCount() const {
    const MapChunk* c = getActiveChunk();
    return c ? c->getColumnCount() : 0;
}
int GESaveData::getActiveChunkRowCount() const {
    const MapChunk* c = getActiveChunk();
    return c ? c->getRowCount() : 0;
}
int GESaveData::getActiveChunkPixelWidth() const {
    const MapChunk* c = getActiveChunk();
    return c ? c->getPixelWidth(_tileWidth) : 0;
}
int GESaveData::getActiveChunkPixelHeight() const {
    const MapChunk* c = getActiveChunk();
    return c ? c->getPixelHeight(_tileHeight) : 0;
}
int GESaveData::getChunkPixelWidth() const { return _chunkColumns * _tileWidth; }
int GESaveData::getChunkPixelHeight() const { return _chunkRows * _tileHeight; }
bool GESaveData::isInfiniteMap() const { return _infiniteMap; }

bool GESaveData::loadGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    clearChunks();
    std::string line;
    int currentRow = 0;
    bool startReadingLayer = false;
    MapChunk chunk;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (!startReadingLayer && line.find("layer") != 0) { _parseKeywordLine(line, chunk); continue; }
        if (line.find("layer 0") == 0) {
            currentRow = 0; startReadingLayer = true;
            if (chunk.columns <= 0 || chunk.rows <= 0) { file.close(); return false; }
            if (!chunk.allocate(chunk.columns, chunk.rows)) { file.close(); return false; }
            continue;
        }

        if (startReadingLayer) {
            if (currentRow >= chunk.rows) { startReadingLayer = false; continue; }
            std::stringstream ss(line);
            std::string cell;
            int x = 0;
            while (std::getline(ss, cell, ',') && x < chunk.columns) {
                int value = 0;
                try { value = std::stoi(cell); }
                catch (...) { value = 0; }
                chunk.tiles[currentRow * chunk.columns + x] = value;
                ++x;
            }
            ++currentRow;
            if (currentRow >= chunk.rows) startReadingLayer = false;
        }
    }

    file.close();
    if (!chunk.isValid()) return false;

    _baseChunk = chunk;
    _chunkColumns = chunk.getColumnCount();
    _chunkRows = chunk.getRowCount();
    if (_infiniteMap && (_chunkColumns <= 0 || _chunkRows <= 0)) _infiniteMap = false;

    ChunkNode* node = ensureChunkNode(_activeChunk);
    if (node) node->chunk = chunk;

    return true;
}

int GESaveData::getTileID(int row, int col) {
    if (_infiniteMap && _chunkColumns > 0 && _chunkRows > 0) {
        ChunkCoord coord;
        int localRow = 0, localCol = 0;
        if (!worldToChunkIndices(row, col, coord, localRow, localCol)) return 0;
        ChunkNode* node = ensureChunkNode(coord);
        if (!node) return 0;
        return node->chunk.getTileID(localRow, localCol);
    }
    const MapChunk* chunk = getActiveChunk();
    return chunk ? chunk->getTileID(row, col) : 0;
}
