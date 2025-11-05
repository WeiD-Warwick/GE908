#include "GEBuffManager.h"

#include <cstdlib>

#include "../../Foundation/GESaveData.h"
#include "../../Foundation/GEWindow.h"
#include "../Camera/GECamera.h"
#include "../Character/GEPlayer.h"

static constexpr float BUFF_LIFETIME_SECONDS = 12.0f;
static constexpr float MIN_SPAWN_INTERVAL = 6.0f;

GEBuffManager::GEBuffManager() {
    for (int i = 0; i < MAX_BUFF_ITEMS; ++i) {
        _buffs[i] = nullptr;
    }
}

GEBuffManager::~GEBuffManager() {
    for (int i = 0; i < MAX_BUFF_ITEMS; ++i) {
        delete _buffs[i];
        _buffs[i] = nullptr;
    }
}

void GEBuffManager::load(GESaveData* saveData) {
    _saveData = saveData;
}

void GEBuffManager::spawnBuff() {
    if (!_saveData) return;

    int freeIndex = -1;
    for (int i = 0; i < MAX_BUFF_ITEMS; ++i) {
        if (!_buffs[i]) {
            freeIndex = i;
            break;
        }
        if (_buffs[i] && !_buffs[i]->isActive()) {
            freeIndex = i;
            break;
        }
    }

    if (freeIndex == -1) return;

    GEBuffType type = (std::rand() % 2 == 0) ? GEBuffType::AttackSpeed : GEBuffType::SkillTargets;
    if (!_buffs[freeIndex]) {
        _buffs[freeIndex] = new GEBuff(type);
    }
    else {
        // recreate with possibly new type
        delete _buffs[freeIndex];
        _buffs[freeIndex] = new GEBuff(type);
    }

    int mapCols = _saveData->getMapColCount();
    int mapRows = _saveData->getMapRowCount();
    int tileW = _saveData->getTileWidth();
    int tileH = _saveData->getTileHeight();

    const int maxAttempts = 30;
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        int col = std::rand() % mapCols;
        int row = std::rand() % mapRows;
        int tileId = _saveData->getTileID(0, row, col);
        if (tileId >= 14 && tileId <= 22) {
            continue; // avoid water tiles
        }

        int worldX = col * tileW + tileW / 2;
        int worldY = row * tileH + tileH / 2;
        _buffs[freeIndex]->activateAt(worldX, worldY, BUFF_LIFETIME_SECONDS);
        return;
    }

    // Failed to find valid position
    delete _buffs[freeIndex];
    _buffs[freeIndex] = nullptr;
}

void GEBuffManager::update(float deltaTime, GEPlayer& player) {
    if (_saveData) {
        _spawnTimer += deltaTime;
        if (_spawnTimer >= _spawnInterval) {
            spawnBuff();
            _spawnTimer = 0.0f;
            _spawnInterval -= 1.0f;
            if (_spawnInterval < MIN_SPAWN_INTERVAL) {
                _spawnInterval = MIN_SPAWN_INTERVAL;
            }
        }
    }

    for (int i = 0; i < MAX_BUFF_ITEMS; ++i) {
        GEBuff* buff = _buffs[i];
        if (!buff || !buff->isActive()) continue;
        buff->tick(deltaTime);
        if (!buff->isActive()) continue;
        if (buff->collide(player)) {
            player.applyBuff(buff->getType());
            buff->deactivate();
        }
    }
}

void GEBuffManager::draw(GEWindow& window, const GECamera& camera) {
    for (int i = 0; i < MAX_BUFF_ITEMS; ++i) {
        GEBuff* buff = _buffs[i];
        if (buff && buff->isActive()) {
            buff->draw(window, camera);
        }
    }
}