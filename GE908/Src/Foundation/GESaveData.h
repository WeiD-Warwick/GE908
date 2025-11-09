#pragma once
#include <string>
#include <fstream>
#include <sstream>

class GESaveData {
public:
    struct ChunkCoord {
        int x = 0;
        int y = 0;

        bool operator==(const ChunkCoord& other) const {
            return x == other.x && y == other.y;
        }
    };

    struct MapChunk {
        int columns = 0;
        int rows = 0;
        int* tiles = nullptr;

        MapChunk() = default;

        ~MapChunk() {
            clear();
        }

        MapChunk(const MapChunk& other) {
            copyFrom(other);
        }

        MapChunk& operator=(const MapChunk& other) {
            if (this != &other) {
                copyFrom(other);
            }
            return *this;
        }

        MapChunk(MapChunk&& other) noexcept {
            moveFrom(other);
        }

        MapChunk& operator=(MapChunk&& other) noexcept {
            if (this != &other) {
                clear();
                moveFrom(other);
            }
            return *this;
        }

        void clear() {
            if (tiles) {
                delete[] tiles;
                tiles = nullptr;
            }
            columns = 0;
            rows = 0;
        }

        bool allocate(int cols, int rowsCount) {
            clear();
            if (cols <= 0 || rowsCount <= 0) return false;
            const int total = cols * rowsCount;
            tiles = new int[total];
            if (!tiles) return false;
            for (int i = 0; i < total; ++i) {
                tiles[i] = 0;
            }
            columns = cols;
            rows = rowsCount;
            return true;
        }

        bool isValid() const {
            return columns > 0 && rows > 0 && tiles != nullptr;
        }

        int getColumnCount() const { return columns; }
        int getRowCount() const { return rows; }

        int getTileID(int row, int col) const {
            if (!isValid()) return 0;
            if (row < 0 || row >= rows || col < 0 || col >= columns) return 0;
            const int index = row * columns + col;
            return tiles[index];
        }

        int getPixelWidth(int tileWidth) const { return columns * tileWidth; }
        int getPixelHeight(int tileHeight) const { return rows * tileHeight; }

    private:
        void copyFrom(const MapChunk& other) {
            if (!other.isValid()) {
                clear();
                return;
            }
            const int total = other.columns * other.rows;
            int* newTiles = new int[total];
            for (int i = 0; i < total; ++i) {
                newTiles[i] = other.tiles[i];
            }
            clear();
            tiles = newTiles;
            columns = other.columns;
            rows = other.rows;
        }

        void moveFrom(MapChunk& other) {
            columns = other.columns;
            rows = other.rows;
            tiles = other.tiles;
            other.columns = 0;
            other.rows = 0;
            other.tiles = nullptr;
        }
    };

private:
    struct ChunkNode {
        ChunkCoord coord;
        MapChunk chunk;
        ChunkNode* next = nullptr;
    };

    int _tileWidth = 0;
    int _tileHeight = 0;

    float _cameraOffsetX = 0;
    float _cameraOffsetY = 0;

    int _windowWidth = 854;
    int _windowHeight = 480;

    MapChunk _baseChunk;
    int _chunkColumns = 0;
    int _chunkRows = 0;
    bool _infiniteMap = false;

    ChunkNode* _chunksHead = nullptr;
    ChunkCoord _activeChunk{ 0, 0 };

    uint32_t _randomSeed = 0u;
    bool _hasRandomSeed = false;


    bool _parseKeywordLine(const std::string& line, MapChunk& chunk) {
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

    void clearChunks() {
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
        _activeChunk = { 0, 0 };
        _randomSeed = 0u;
        _hasRandomSeed = false;
    }

    ChunkNode* findChunkNode(const ChunkCoord& coord) {
        ChunkNode* node = _chunksHead;
        while (node) {
            if (node->coord == coord) return node;
            node = node->next;
        }
        return nullptr;
    }

    const ChunkNode* findChunkNode(const ChunkCoord& coord) const {
        const ChunkNode* node = _chunksHead;
        while (node) {
            if (node->coord == coord) return node;
            node = node->next;
        }
        return nullptr;
    }

    static int floorDiv(int value, int divisor) {
        int quotient = value / divisor;
        int remainder = value % divisor;
        if ((remainder != 0) && ((remainder < 0) != (divisor < 0))) {
            --quotient;
        }
        return quotient;
    }

    static int positiveMod(int value, int divisor) {
        int mod = value % divisor;
        if (mod < 0) mod += std::abs(divisor);
        return mod;
    }

    bool worldToChunkIndices(int tileRow, int tileCol, ChunkCoord& coord, int& localRow, int& localCol) const {
        if (_chunkColumns <= 0 || _chunkRows <= 0) return false;
        coord.x = floorDiv(tileCol, _chunkColumns);
        coord.y = floorDiv(tileRow, _chunkRows);
        localCol = positiveMod(tileCol, _chunkColumns);
        localRow = positiveMod(tileRow, _chunkRows);
        return true;
    }

    uint32_t hashCoordinates(int worldRow, int worldCol) const {
        uint64_t hash = static_cast<uint64_t>(_randomSeed);
        hash ^= 0x9e3779b97f4a7c15ULL + static_cast<uint64_t>(worldRow) + (hash << 6) + (hash >> 2);
        hash ^= 0x9e3779b97f4a7c15ULL + static_cast<uint64_t>(worldCol) + (hash << 6) + (hash >> 2);
        hash ^= hash >> 32;
        return static_cast<uint32_t>(hash);
    }

    int sampleTileForWorld(int worldRow, int worldCol) const {
        if (!_baseChunk.isValid()) return 0;
        if (!_hasRandomSeed) {
            // Default to repeating base chunk when no seed is provided.
            const int srcRow = positiveMod(worldRow, _chunkRows);
            const int srcCol = positiveMod(worldCol, _chunkColumns);
            return _baseChunk.getTileID(srcRow, srcCol);
        }

        const uint32_t hashed = hashCoordinates(worldRow, worldCol);
        const int srcRow = static_cast<int>(hashed % static_cast<uint32_t>(_chunkRows));
        const int srcCol = static_cast<int>((hashed / static_cast<uint32_t>(_chunkRows)) % static_cast<uint32_t>(_chunkColumns));
        return _baseChunk.getTileID(srcRow, srcCol);
    }

    void generateChunk(MapChunk& chunk, const ChunkCoord& coord) {
        if (!_baseChunk.isValid()) return;

        if (coord.x == 0 && coord.y == 0) {
            chunk = _baseChunk;
            return;
        }

        if (!chunk.allocate(_chunkColumns, _chunkRows)) return;

        for (int row = 0; row < _chunkRows; ++row) {
            for (int col = 0; col < _chunkColumns; ++col) {
                const int worldRow = coord.y * _chunkRows + row;
                const int worldCol = coord.x * _chunkColumns + col;
                const int tile = sampleTileForWorld(worldRow, worldCol);
                const int index = row * _chunkColumns + col;
                chunk.tiles[index] = tile;
            }
        }
    }

    ChunkNode* ensureChunkNode(const ChunkCoord& coord) {
        ChunkNode* node = findChunkNode(coord);
        if (node) return node;
        if (!_infiniteMap && (coord.x != 0 || coord.y != 0)) return nullptr;
        node = new ChunkNode();
        node->coord = coord;
        node->next = _chunksHead;
        _chunksHead = node;
        if (_infiniteMap) {
            generateChunk(node->chunk, coord);
        }
        else if (_baseChunk.isValid()) {
            node->chunk = _baseChunk;
        }
        return node;
    }

public:
    GESaveData() = default;
    ~GESaveData() {
       clearChunks();
    }

    GESaveData(const GESaveData&) = delete;
    GESaveData& operator=(const GESaveData&) = delete;

    int getTileWidth() const { return _tileWidth; }
    int getTileHeight() const { return _tileHeight; }

    void setCameraOffset(int x, int y) { _cameraOffsetX = static_cast<float>(x); _cameraOffsetY = static_cast<float>(y); }
    float getCameraOffsetX() { return _cameraOffsetX; }
    float getCameraOffsetY() { return _cameraOffsetY; }

    void setWindowSize(int width, int height) { _windowWidth = width; _windowHeight = height; }
    int getScreenWidth() const { return _windowWidth; }
    int getScreenHeight() const { return _windowHeight; }

    void setActiveChunk(int chunkX, int chunkY) {
        if (!_infiniteMap) {
            _activeChunk = { 0, 0 };
            ensureChunkNode(_activeChunk);
            return;
        }

        _activeChunk.x = chunkX;
        _activeChunk.y = chunkY;

        ensureChunkNode(_activeChunk);
    }

    void updateActiveChunkFromWorldPosition(float worldX, float worldY) {
        if (!_infiniteMap) return;
        if (_tileWidth <= 0 || _tileHeight <= 0) return;
        const int tileCol = static_cast<int>(std::floor(worldX / _tileWidth));
        const int tileRow = static_cast<int>(std::floor(worldY / _tileHeight));
        ChunkCoord coord;
        int localRow = 0;
        int localCol = 0;
        if (!worldToChunkIndices(tileRow, tileCol, coord, localRow, localCol)) return;
        setActiveChunk(coord.x, coord.y);
    }

    const MapChunk* getActiveChunk() const {
        const ChunkNode* node = findChunkNode(_activeChunk);
        if (!node) return nullptr;
        return &node->chunk;
    }

    MapChunk* getActiveChunk() {
        ChunkNode* node = findChunkNode(_activeChunk);
        if (!node) {
            if (!_infiniteMap && _chunksHead) {
                return &_chunksHead->chunk;
            }
            return nullptr;
        }
        return &node->chunk;
    }

    int getActiveChunkColumnCount() const {
        const MapChunk* chunk = getActiveChunk();
        return chunk ? chunk->getColumnCount() : 0;
    }

    int getActiveChunkRowCount() const {
        const MapChunk* chunk = getActiveChunk();
        return chunk ? chunk->getRowCount() : 0;
    }

    int getActiveChunkPixelWidth() const {
        const MapChunk* chunk = getActiveChunk();
        return chunk ? chunk->getPixelWidth(_tileWidth) : 0;
    }

    int getActiveChunkPixelHeight() const {
        const MapChunk* chunk = getActiveChunk();
        return chunk ? chunk->getPixelHeight(_tileHeight) : 0;
    }

    int getChunkPixelWidth() const { return _chunkColumns * _tileWidth; }
    int getChunkPixelHeight() const { return _chunkRows * _tileHeight; }

    bool isInfiniteMap() const { return _infiniteMap; }

    bool loadGame(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        clearChunks();

        std::string line;
        int currentRow = 0;
        bool startReadingLayer = false;
        MapChunk chunk;

        while (std::getline(file, line)) {
            if (line.empty()) continue;
            if (!startReadingLayer && line.find("layer") != 0) {
                _parseKeywordLine(line, chunk);
                continue;
            }

            if (line.find("layer 0") == 0) {
                currentRow = 0;
                startReadingLayer = true;
                if (chunk.columns <= 0 || chunk.rows <= 0) {
                    file.close();
                    return false;
                }
                if (!chunk.allocate(chunk.columns, chunk.rows)) {
                    file.close();
                    return false;
                }
                continue;
            }

            if (startReadingLayer) {
                if (currentRow >= chunk.rows) {
                    startReadingLayer = false;
                    continue;
                }

                std::stringstream ss(line);
                std::string cell;
                int x = 0;

                while (std::getline(ss, cell, ',') && x < chunk.columns) {
                    int value = 0;
                    try {
                        value = std::stoi(cell);
                    }
                    catch (...) {
                        // Set to 0 by default if parsing fails
                        value = 0;
                    }
                    const int index = currentRow * chunk.columns + x;
                    chunk.tiles[index] = value;
                    ++x;
                }

                ++currentRow;
                if (currentRow >= chunk.rows) {
                    startReadingLayer = false;
                }
            }
        }

        file.close();


        if (!chunk.isValid()) return false;

        _baseChunk = chunk;
        _chunkColumns = chunk.getColumnCount();
        _chunkRows = chunk.getRowCount();
        if (_infiniteMap && (_chunkColumns <= 0 || _chunkRows <= 0)) {
            _infiniteMap = false;
        }

        ChunkNode* node = ensureChunkNode(_activeChunk);
        node->chunk = chunk;

        if (node) {
            node->chunk = chunk;
        }

        return true;
    }

    int getTileID(int row, int col) {
        if (_infiniteMap && _chunkColumns > 0 && _chunkRows > 0) {
            ChunkCoord coord;
            int localRow = 0;
            int localCol = 0;
            if (!worldToChunkIndices(row, col, coord, localRow, localCol)) return 0;
            ChunkNode* node = ensureChunkNode(coord);
            if (!node) return 0;
            return node->chunk.getTileID(localRow, localCol);
        }

        const MapChunk* chunk = getActiveChunk();
        if (!chunk) return 0;
        return chunk->getTileID(row, col);
    }
};