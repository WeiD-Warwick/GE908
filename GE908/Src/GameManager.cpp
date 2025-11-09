#include "GameManager.h"
#include "Foundation/GEDebug.h"
#include <string>

#define WINDOW_WIDTH 854
#define WINDOW_HEIGHT 480

using namespace GamesEngineeringBase;

void GameManager::run() {
    _isRunning = true;
    _font.load();
    _mapProvider.loadTileResources("Src/Assets/MapTiles/");

    // check use which map
    // 1: FixedMap 
    // 2: InfiniteMap
    bool pressKey = false;
    bool isFixedMap = true;
    while (!pressKey) {
        _window.checkInput();

        if (_window.keyPressed('1')) {
            isFixedMap = true;
            pressKey = true;
        }
        if (_window.keyPressed('2')) {
            isFixedMap = false;
            pressKey = true;
        }

        _window.clear();
        _font.draw("Press 1 for Fixed Map", GEPoint(200, 200), RED, _window);
        _font.draw("Press 2 for Infinite Map", GEPoint(200, 240), RED, _window);
        _window.present();
    }

    const std::string& dataPath = isFixedMap ? "Src/SaveGames/fixed.txt" : "Src/SaveGames/inifinty.txt";

    loadComponent(dataPath);

    while (_isRunning) {
        GEFrameTimer::shared().beginFrame();
        float deltaTime = GEFrameTimer::shared().getDeltaTime();

        update(deltaTime);
        render();

        GEFrameTimer::shared().endFrame();
    }
}

void GameManager::loadComponent(const std::string& dataPath) {
    _saveData = new GESaveData();
    _saveData->loadGame(dataPath);

    if (!_saveData) {
        _isRunning = false;
        return;
    }

    _saveData->setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    _player.bind(_ctx);

    if (_saveData->isInfiniteMap()) {
        _camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, -1, -1);
    }
    else {
        int mapWorldWidth = _saveData->getActiveChunkPixelWidth() > 0 ? _saveData->getActiveChunkPixelWidth() : WINDOW_WIDTH;
        int mapWorldHeight = _saveData->getActiveChunkPixelHeight() > 0 ? _saveData->getActiveChunkPixelHeight() : WINDOW_HEIGHT;
        _camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);
    }
    _camera.setPosition(_saveData->getCameraOffsetX(), _saveData->getCameraOffsetY());
    _mapProvider.load(_saveData);
    _enemyProvider.load(_saveData);
    _powerUpProvider.load(_saveData);
    _projectileProvider.load(_saveData);
}

void GameManager::update(float deltaTime) {
    _window.checkInput();
    GEDebug::shared().updateFromInput(_window);

    _player.update(deltaTime, _window);

    auto& body = _player.collisionBody();
    _camera.followPlayer(body.getOriginX(), body.getOriginY(), body.getWidth(), body.getHeight());

    if (_saveData) {
        _saveData->setCameraOffset(_camera.getX(), _camera.getY());
        _saveData->updateActiveChunkFromWorldPosition(body.getCenterX(), body.getCenterY());
    }

    _enemyProvider.update(deltaTime, _ctx);
    _projectileProvider.update(deltaTime, _ctx);
    _powerUpProvider.update(deltaTime, _ctx);

    if (_saveData && _window.keyPressed('P')) {
        GEPlayerState playerState = _player.snapshotState();
        GEEnemyManagerState enemyManagerState = _enemyProvider.snapshotState();
        GEProjectileManagerState projectileState = _projectileProvider.snapshotState();
        GEPowerUpManagerState powerUpState = _powerUpProvider.snapshotState();
        _saveData->saveState(& playerState, &enemyManagerState, &projectileState, &powerUpState);
    }
}

void GameManager::render() {
    _window.clear();

    _mapProvider.draw(_window, _camera);
    _enemyProvider.draw(_window, _camera);
    _projectileProvider.draw(_window, _camera);
    _player.draw(_window, _camera);
    _powerUpProvider.draw(_window, _camera);

    drawText();
    _window.present();
}

void GameManager::drawText() {
    int y = 20;
    _font.draw("Normal: " + std::to_string(_enemyProvider.getKillCount(GEEnemyType::Normal)),
        GEPoint(20, y), RED, _window);
    y += 20;
    _font.draw("Fast: " + std::to_string(_enemyProvider.getKillCount(GEEnemyType::Fast)),
        GEPoint(20, y), RED, _window);
    y += 20;
    _font.draw("Heavy: " + std::to_string(_enemyProvider.getKillCount(GEEnemyType::Heavy)),
        GEPoint(20, y), RED, _window);
    y += 20;
    _font.draw("Static: " + std::to_string(_enemyProvider.getKillCount(GEEnemyType::StaticShooter)),
        GEPoint(20, y), RED, _window);

    _font.draw("FPS: " + std::to_string(static_cast<int>(GEFrameTimer::shared().getFPS())),
        GEPoint(20, 400), RED, _window);
    _font.draw("HP: " + std::to_string(_player.getHP()),
        GEPoint(200, 400), RED, _window);
    _font.draw("Skill: " + std::to_string(static_cast<int>(_player.getAOECooldownTime())),
        GEPoint(400, 400), RED, _window);
}

void GameManager::stop() {
    _isRunning = false;
}
