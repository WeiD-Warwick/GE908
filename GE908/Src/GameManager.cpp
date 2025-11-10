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

    while (_isRunning) {
        GEFrameTimer::shared().beginFrame();
        float deltaTime = GEFrameTimer::shared().getDeltaTime();

        switch (_gameState) {
        case GEGameLifeCircle::Menu:
            _levelTimeRemaining = 120.0f;
            updateMenu();
            renderMenu();
            break;
        case GEGameLifeCircle::Playing:
            if (!_componentHasLoaded) {
                loadComponent(_loadMode);
                _componentHasLoaded = true;
            }
            update(deltaTime);
            render();
            _levelTimeRemaining -= deltaTime;
            break;
        default:
            updateEnding();
            renderEnding();
            break;
        }
        GEFrameTimer::shared().endFrame();
    }
}

void GameManager::loadComponent(const GEDataLoadMode loadMode) {
    _saveData = new GESaveData();
    _saveData->loadGame(loadMode);

    if (!_saveData) {
        _isRunning = false;
        return;
    }

    const bool loadingFromSave = (loadMode == GEDataLoadMode::LastSavedFix || loadMode == GEDataLoadMode::LastSavedInfinite);
    if (loadingFromSave && _saveData->hasLevelTimeRemaining())
        _levelTimeRemaining = _saveData->getLevelTimeRemaining();
    else
        _levelTimeRemaining = 120.0f;

    _saveData->setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    _mapProvider.load(_saveData);

    _player.bind(_ctx);

    int mapWorldWidth = _saveData->isInfiniteMap() ? -1 : (_saveData->getActiveChunkPixelWidth() > 0 ? _saveData->getActiveChunkPixelWidth() : WINDOW_WIDTH);
    int mapWorldHeight = _saveData->isInfiniteMap() ? -1 : (_saveData->getActiveChunkPixelHeight() > 0 ? _saveData->getActiveChunkPixelHeight() : WINDOW_HEIGHT);
    _camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);
    _camera.setPosition(_saveData->getCameraOffsetX(), _saveData->getCameraOffsetY());

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
        _saveData->setLevelTimeRemaining(_levelTimeRemaining);
    }

    _enemyProvider.update(deltaTime, _ctx);
    _projectileProvider.update(deltaTime, _ctx);
    _powerUpProvider.update(deltaTime, _ctx);

    if (_player.getHP() <= 0) {
        _gameState = GEGameLifeCircle::Defeat;
    }

    if (_player.getHP() > 0 && _levelTimeRemaining <= 0) {
        _gameState = GEGameLifeCircle::Victory;
    }

    if (_saveData && _window.keyPressed('L')) {
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
        GEPoint(20, 420), RED, _window);
    _font.draw("Skill: " + std::to_string(static_cast<int>(_player.getAOECooldownTime())),
        GEPoint(20, y=440), RED, _window);

    _font.draw("Time: " + std::to_string(static_cast<int>(std::ceil(_levelTimeRemaining))),
        GEPoint(540, 20), RED, _window);
    _font.draw("J  Show Collidars.", GEPoint(540, 400), BLUE, _window);
    _font.draw("K  Show AOE Range.", GEPoint(540, 420), BLUE, _window);
    _font.draw("L  Save Game.", GEPoint(540, 440), BLUE, _window);
}

void GameManager::stop() {
    _isRunning = false;
}

void GameManager::updateMenu() {
    _window.checkInput();
    if (_window.keyPressed('1')) {
        _gameState = GEGameLifeCircle::Playing;
        _loadMode = GEDataLoadMode::NewFix;
    }
    else if (_window.keyPressed('2')) {
        _gameState = GEGameLifeCircle::Playing;
        _loadMode = GEDataLoadMode::NewInfinite;
    }
    else if (_window.keyPressed('3')) {
        _gameState = GEGameLifeCircle::Playing;
        _loadMode = GEDataLoadMode::LastSavedFix;
    }
    else if (_window.keyPressed('4')) {
        _gameState = GEGameLifeCircle::Playing;
        _loadMode = GEDataLoadMode::LastSavedInfinite;
    }
}


void GameManager::renderMenu() {
    _window.clear();
    _font.draw("Press 1 for new Fixed Map", GEPoint(160, 160), WHITE, _window);
    _font.draw("Press 2 for new Infinite Map", GEPoint(160, 200), WHITE, _window);
    _font.draw("Press 3 for last saved Fixed Map", GEPoint(160, 240), WHITE, _window);
    _font.draw("Press 4 for last saved Infinite Map", GEPoint(160, 280), WHITE, _window);
    _window.present();
}


void GameManager::updateEnding() {
    if (_window.keyPressed(' ')) {
        delete _saveData;
        _saveData = new GESaveData();

        _gameState = GEGameLifeCircle::Menu;
        _levelTimeRemaining = 120.0f;
        _componentHasLoaded = false;
    }
}

void GameManager::renderEnding() {
    _window.clear();
    if (_gameState == GEGameLifeCircle::Defeat) {
        _font.draw("Failed", GEPoint(200, 200), RED, _window);
        _font.draw("Press Space Back To Menu.", GEPoint(200, 240), WHITE, _window);
    }

    if (_gameState == GEGameLifeCircle::Victory) {
        _font.draw("Win!", GEPoint(200, 200), GREEN, _window);
        _font.draw("Press Space Back To Menu.", GEPoint(200, 240), WHITE, _window);
    }
    
    _window.present();
}