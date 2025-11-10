#include "GESaveData.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GEConst.h"

GESaveData::GESaveData() = default;
GESaveData::~GESaveData() { clearChunks(); }

bool GESaveData::_parseKeywordLine(const std::string& line, MapChunk& chunk) {
    if (line.empty()) return false;
    std::istringstream iss(line);
    std::string key;
    if (!(iss >> key)) return false;

    if (key == "seed") {
        uint32_t flagOrSeed = 0u;
        uint32_t value = 0u;
        if (!(iss >> flagOrSeed)) return false;
        if (iss >> value) {
            _hasRandomSeed = (flagOrSeed != 0u);
            _randomSeed = value;
        }
        else {
            _randomSeed = flagOrSeed;
            _hasRandomSeed = true;
        }
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
    _baseChunk.clear();
    _chunkColumns = 0;
    _chunkRows = 0;
    _infiniteMap = false;
    _activeChunk = { 0,0 };
    _randomSeed = 0u;
    _hasRandomSeed = false;
    _loadedPlayerState.reset();
    _loadedEnemyManagerState.reset();
    _loadedProjectileManagerState.reset();
    _loadedPowerUpManagerState.reset();
    _stateFilePath.clear();
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
    if (!_infiniteMap) {
        _activeChunk = { 0,0 };
        return;
    }
    _activeChunk.x = chunkX;
    _activeChunk.y = chunkY;
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
    return _baseChunk.isValid() ? &_baseChunk : nullptr;
}

MapChunk* GESaveData::getActiveChunk() {
    return _baseChunk.isValid() ? &_baseChunk : nullptr;
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

bool GESaveData::loadGame(const GEDataLoadMode loadMode) {
    std::string filename = getFilePath(loadMode);
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    clearChunks();

    const bool infiniteMode = (loadMode == GEDataLoadMode::NewInfinite || loadMode == GEDataLoadMode::LastSavedInfinite);
    _infiniteMap = infiniteMode;
    _stateFilePath = filename + (infiniteMode ? ".ini.state" : ".fix.state");


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
    if (_chunkColumns <= 0 || _chunkRows <= 0) _infiniteMap = false;

    if (_infiniteMap && !_hasRandomSeed) {
        _randomSeed = static_cast<uint32_t>(std::rand());
        _hasRandomSeed = true;
    }

    _activeChunk = { 0,0 };
    if (loadMode == GEDataLoadMode::LastSavedFix || loadMode == GEDataLoadMode::LastSavedInfinite)
        loadState(_stateFilePath);
    return true;
}

std::string GESaveData::getFilePath(const GEDataLoadMode loadMode) {
    return "Src/SaveGames/tilt.txt";
}

int GESaveData::getTileID(int row, int col) {
    if (_infiniteMap && _chunkColumns > 0 && _chunkRows > 0)
        return sampleTileForWorld(row, col);
    const MapChunk* chunk = getActiveChunk();
    return chunk ? chunk->getTileID(row, col) : 0;
}

const GEPlayerState* GESaveData::getPlayerState() const {
    return _loadedPlayerState.get();
}

const GEEnemyManagerState* GESaveData::getEnemyManagerState() const {
    return _loadedEnemyManagerState.get();
}

const GEProjectileManagerState* GESaveData::getProjectileManagerState() const {
    return _loadedProjectileManagerState.get();
}

const GEPowerUpManagerState* GESaveData::getPowerUpManagerState() const {
    return _loadedPowerUpManagerState.get();
}

bool GESaveData::saveState(const GEPlayerState* playerState,
                           const GEEnemyManagerState* enemyManagerState,
                           const GEProjectileManagerState* projectileManagerState,
                           const GEPowerUpManagerState* powerUpManagerState) const {
    if (_stateFilePath.empty()) return false;
    return saveState(_stateFilePath, playerState, enemyManagerState, projectileManagerState, powerUpManagerState);
}

bool GESaveData::saveState(const std::string& filename,
                           const GEPlayerState* playerState,
                           const GEEnemyManagerState* enemyManagerState,
                           const GEProjectileManagerState* projectileManagerState,
                           const GEPowerUpManagerState* powerUpManagerState) const {

    if (filename.empty()) return false;
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "# GE runtime state\n";
    file << "window " << _windowWidth << ' ' << _windowHeight << "\n";
    file << "camera " << _cameraOffsetX << ' ' << _cameraOffsetY << "\n";
    file << "chunk " << _activeChunk.x << ' ' << _activeChunk.y << "\n";
    file << "seed " << (_hasRandomSeed ? 1 : 0) << ' ' << _randomSeed << "\n";

    if (playerState) {
        const auto& p = *playerState;
        file << "player "
            << p.centerX << ' ' << p.centerY << ' '
            << p.hp << ' ' << p.maxHp << ' ' << p.speed << ' '
            << p.autoAttackTimer << ' ' << p.autoAttackSpeedMultiplier << ' '
            << p.aoeCooldownTimer << ' ' << p.aoeCooldown << ' '
            << p.contactDamageCooldownTimer << ' '
            << p.aoeTargetCount << ' ' << (p.aoeKeyHeld ? 1 : 0) << "\n";
    }

    if (enemyManagerState) {
        file << "enemytimers "
            << enemyManagerState->spawnTimer << ' '
            << enemyManagerState->spawnInterval << ' '
            << enemyManagerState->difficultyTimer << ' '
            << enemyManagerState->elapsedTime << "\n";

        file << "killcounts";
        for (int value : enemyManagerState->killCounts) file << ' ' << value;
        file << "\n";
    }
    else {
        file << "enemytimers 0 0 0 0\n";
        file << "killcounts 0 0 0 0\n";
    }

    const std::size_t enemyCount = enemyManagerState ? enemyManagerState->enemyCount() : 0;
    file << "enemies " << enemyCount << "\n";
    if (enemyManagerState) {
        enemyManagerState->forEachEnemyState([&](const GEEnemyState& enemy) {
            file << "enemy "
                << static_cast<int>(enemy.type) << ' '
                << enemy.centerX << ' ' << enemy.centerY << ' '
                << enemy.hp << ' ' << enemy.maxHp << ' '
                << enemy.attackCooldown << "\n";
            });
    }

    const std::size_t projectileCount = projectileManagerState ? projectileManagerState->projectileCount() : 0;
    file << "projectiles " << projectileCount << "\n";
    if (projectileManagerState) {
        projectileManagerState->forEachProjectile([&](const GEProjectileState& projectile) {
            file << "projectile "
                << static_cast<int>(projectile.owner) << ' '
                << projectile.centerX << ' ' << projectile.centerY << ' '
                << projectile.dirX << ' ' << projectile.dirY << ' '
                << projectile.speed << ' ' << projectile.damage << "\n";
            });
    }

    const std::size_t powerUpCount = powerUpManagerState ? powerUpManagerState->powerUpCount() : 0;
    float powerUpTimer = powerUpManagerState ? powerUpManagerState->spawnTimer : 0.0f;
    file << "poweruptimer " << powerUpTimer << "\n";
    file << "powerups " << powerUpCount << "\n";
    if (powerUpManagerState) {
        powerUpManagerState->forEachPowerUp([&](const GEPowerUpState& powerUp) {
            file << "powerup "
                << static_cast<int>(powerUp.type) << ' '
                << powerUp.centerX << ' ' << powerUp.centerY << ' '
                << powerUp.remainingTime << ' ' << powerUp.timeToLive << "\n";
            });
    }

    file.close();
    return true;
}

bool GESaveData::loadState() {
    if (_stateFilePath.empty()) return false;
    return loadState(_stateFilePath);
}

bool GESaveData::loadState(const std::string& filename) {
    if (filename.empty()) return false;
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    _stateFilePath = filename;
    _loadedPlayerState.reset();
    _loadedEnemyManagerState.reset();
    _loadedProjectileManagerState.reset();
    _loadedPowerUpManagerState.reset();

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)) continue;

        if (key == "window") {
            iss >> _windowWidth >> _windowHeight;
        }
        else if (key == "camera") {
            iss >> _cameraOffsetX >> _cameraOffsetY;
        }
        else if (key == "chunk") {
            int cx = 0, cy = 0;
            if (iss >> cx >> cy) {
                if (_infiniteMap) setActiveChunk(cx, cy);
            }
        }
        else if (key == "seed") {
            unsigned int flagOrSeed = 0u;
            unsigned int value = 0u;
            if (iss >> flagOrSeed) {
                if (iss >> value) {
                    _hasRandomSeed = (flagOrSeed != 0u);
                    _randomSeed = value;
                }
                else {
                    _randomSeed = flagOrSeed;
                    _hasRandomSeed = true;
                }
            }
        }
        else if (key == "player") {
            GEPlayerState state;
            int aoeHeld = 0;
            if (iss >> state.centerX >> state.centerY >> state.hp >> state.maxHp >> state.speed
                >> state.autoAttackTimer >> state.autoAttackSpeedMultiplier
                >> state.aoeCooldownTimer >> state.aoeCooldown >> state.contactDamageCooldownTimer) {
                if (iss >> state.aoeTargetCount >> aoeHeld) {
                    state.aoeKeyHeld = (aoeHeld != 0);
                }
                else {
                    state.aoeTargetCount = Player::PLAYER_MAX_AOE_TARGETS;
                    state.aoeKeyHeld = false;
                }
                _loadedPlayerState = std::make_unique<GEPlayerState>(state);
            }
        }
        else if (key == "enemytimers") {
            GEEnemyManagerState state;
            if (_loadedEnemyManagerState) state = *_loadedEnemyManagerState;
            if (iss >> state.spawnTimer
                >> state.spawnInterval
                >> state.difficultyTimer
                >> state.elapsedTime) {
                _loadedEnemyManagerState = std::make_unique<GEEnemyManagerState>(state);
            }
        }
        else if (key == "killcounts") {
            GEEnemyManagerState state;
            if (_loadedEnemyManagerState) state = *_loadedEnemyManagerState;
            for (int& value : state.killCounts) value = 0;
            int value = 0;
            size_t index = 0;
            while (iss >> value && index < state.killCounts.size()) {
                state.killCounts[index++] = value;
            }
            _loadedEnemyManagerState = std::make_unique<GEEnemyManagerState>(state);
        }
        else if (key == "enemies") {
            // count hint is optional, nothing to do.
        }
        else if (key == "enemy") {
            GEEnemyState enemyState{};
            int typeInt = 0;
            if (iss >> typeInt
                >> enemyState.centerX >> enemyState.centerY
                >> enemyState.hp >> enemyState.maxHp
                >> enemyState.attackCooldown) {

                enemyState.type = static_cast<GEEnemyType>(typeInt);
                enemyState.activate();
                if (!_loadedEnemyManagerState)
                    _loadedEnemyManagerState = std::make_unique<GEEnemyManagerState>();
                _loadedEnemyManagerState->addEnemyState(enemyState);
            }
        }
        else if (key == "projectiles") {
            // count hint optional
        }
        else if (key == "projectile") {
            GEProjectileState projectileState;
            int ownerInt = 0;
            if (iss >> ownerInt >> projectileState.centerX >> projectileState.centerY
                >> projectileState.dirX >> projectileState.dirY
                >> projectileState.speed >> projectileState.damage) {
                projectileState.owner = static_cast<ProjectileOwner>(ownerInt);
                projectileState.activate();
                if (!_loadedProjectileManagerState)
                    _loadedProjectileManagerState = std::make_unique<GEProjectileManagerState>();
                _loadedProjectileManagerState->addProjectileState(projectileState);
            }
        }
        else if (key == "poweruptimer") {
            float timer = 0.0f;
            if (iss >> timer) {
                if (!_loadedPowerUpManagerState)
                    _loadedPowerUpManagerState = std::make_unique<GEPowerUpManagerState>();
                _loadedPowerUpManagerState->spawnTimer = timer;
            }
        }
        else if (key == "powerups") {
            // count hint optional
        }
        else if (key == "powerup") {
            GEPowerUpState powerUpState;
            int typeInt = 0;
            if (iss >> typeInt >> powerUpState.centerX >> powerUpState.centerY
                >> powerUpState.remainingTime >> powerUpState.timeToLive) {
                powerUpState.type = static_cast<GEPowerUpType>(typeInt);
                powerUpState.activate();
                if (!_loadedPowerUpManagerState)
                    _loadedPowerUpManagerState = std::make_unique<GEPowerUpManagerState>();
                _loadedPowerUpManagerState->addPowerUpState(powerUpState);
            }
        }
    }

    file.close();
    return true;
}
