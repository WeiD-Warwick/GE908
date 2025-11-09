#include "GameManager.h"
#include "Foundation/GEDebug.h"
#include <string>

#define WINDOW_WIDTH 854
#define WINDOW_HEIGHT 480

using namespace GamesEngineeringBase;

void GameManager::run() {
    _isRunning = true;
    loadComponent();

    while (_isRunning) { 
        GEFrameTimer::shared().beginFrame();
        float deltaTime = GEFrameTimer::shared().getDeltaTime();

        update(deltaTime);
        render();

        GEFrameTimer::shared().endFrame(); 
    }
}

void GameManager::loadComponent() {
    _font.load();
    _mapProvider.load("Src/Assets/MapTiles/", "Src/SaveGames/tiles.txt");

    GESaveData* saveData = _mapProvider.getSaveData();

    if (!saveData) return;

    _player.bind(_ctx);
    int mapWorldWidth = saveData->getActiveChunkPixelWidth();
    int mapWorldHeight = saveData->getActiveChunkPixelHeight();
    if (mapWorldWidth <= 0) mapWorldWidth = WINDOW_WIDTH;
    if (mapWorldHeight <= 0) mapWorldHeight = WINDOW_HEIGHT;

    _camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);
    _enemyProvider.load(saveData);
    _powerUpProvider.load(saveData);
}

void GameManager::update(float deltaTime) {
    _window.checkInput();
    GEDebug::shared().updateFromInput(_window);

    // 玩家逻辑
    _player.update(deltaTime, _window);

    // 相机跟随玩家
    auto& body = _player.collisionBody();
    _camera.followPlayer(body.getOriginX(), body.getOriginY(),
        body.getWidth(), body.getHeight());

    // 同步相机偏移到存档
    GESaveData* saveData = _mapProvider.getSaveData();
    if (saveData)
        saveData->setCameraOffset(_camera.getX(), _camera.getY());

    // 敌人、投射物、道具逻辑
    _enemyProvider.update(deltaTime, _ctx);
    _projectileProvider.update(deltaTime, _ctx);
    _powerUpProvider.update(deltaTime, _ctx);
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
