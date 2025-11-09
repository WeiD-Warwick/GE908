#pragma once
#include <string>
#include <memory>
#include "MapChunk.h"
#include "GEGameState.h"

class GESaveData {

private:
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

    std::string _stateFilePath;
    std::unique_ptr<GEPlayerState> _loadedPlayerState;
    std::unique_ptr<GEEnemyManagerState> _loadedEnemyManagerState;
    std::unique_ptr<GEProjectileManagerState> _loadedProjectileManagerState;
    std::unique_ptr<GEPowerUpManagerState> _loadedPowerUpManagerState;

private:
    bool _parseKeywordLine(const std::string& line, MapChunk& chunk);
    void clearChunks();
    ChunkNode* findChunkNode(const ChunkCoord& coord);
    const ChunkNode* findChunkNode(const ChunkCoord& coord) const;
    static int floorDiv(int value, int divisor);
    static int positiveMod(int value, int divisor);
    bool worldToChunkIndices(int tileRow, int tileCol, ChunkCoord& coord, int& localRow, int& localCol) const;
    uint32_t hashCoordinates(int worldRow, int worldCol) const;
    int sampleTileForWorld(int worldRow, int worldCol) const;
    void generateChunk(MapChunk& chunk, const ChunkCoord& coord);
    ChunkNode* ensureChunkNode(const ChunkCoord& coord);

public:
    GESaveData();
    ~GESaveData();

    GESaveData(const GESaveData&) = delete;
    GESaveData& operator=(const GESaveData&) = delete;

    int getTileWidth() const { return _tileWidth; }
    int getTileHeight() const { return _tileHeight; }

    void setCameraOffset(int x, int y);
    float getCameraOffsetX();
    float getCameraOffsetY();

    void setWindowSize(int width, int height);
    int getScreenWidth() const;
    int getScreenHeight() const;

    void setActiveChunk(int chunkX, int chunkY);
    void updateActiveChunkFromWorldPosition(float worldX, float worldY);

    const MapChunk* getActiveChunk() const;
    MapChunk* getActiveChunk();

    int getActiveChunkColumnCount() const;
    int getActiveChunkRowCount() const;
    int getActiveChunkPixelWidth() const;
    int getActiveChunkPixelHeight() const;

    int getChunkPixelWidth() const;
    int getChunkPixelHeight() const;

    bool isInfiniteMap() const;

    bool loadGame(const std::string& filename);
    int getTileID(int row, int col);

    const GEPlayerState* getPlayerState() const;
    const GEEnemyManagerState* getEnemyManagerState() const;
    const GEProjectileManagerState* getProjectileManagerState() const;
    const GEPowerUpManagerState* getPowerUpManagerState() const;

    bool saveState(const GEPlayerState* playerState,
                   const GEEnemyManagerState* enemyManagerState,
                   const GEProjectileManagerState* projectileManagerState,
                   const GEPowerUpManagerState* powerUpManagerState) const;

    bool saveState(const std::string& filename, 
                   const GEPlayerState* playerState, 
                   const GEEnemyManagerState* enemyManagerState,
                   const GEProjectileManagerState* projectileManagerState,
                   const GEPowerUpManagerState* powerUpManagerState) const;

    bool loadState();
    bool loadState(const std::string& filename);
    const std::string& getStateFilePath() const { return _stateFilePath; }
};
