#include <cmath>
#include "GEPlayer.h"
#include "GECharacter.h"
#include "GEEnemy.h"
#include "GEMapsManager.h"
#include "../Foundation/GESaveData.h"
#include "../Foundation/GEDebug.h"

static float fireTimer = 0.0f;
static constexpr float FIRE_DAMAGE_INTERVAL = 1.0f;
static constexpr int FIRE_DAMAGE = 15;

GEPlayer::GEPlayer()
    : GECharacter("Src/Assets/Textures/player.png", GECollisionType::Player) {
    _hp = 200;
    _speed = 240;
    _maxHp = _hp;

    setContactDamageCooldownDuration(0.5f);

    for (int i = 0;i < PLAYER_MAX_AOE_EFFECTS;i++) {
        _aoeEffects[i].active = false;
        _aoeEffects[i].centerX = 0.0f;
        _aoeEffects[i].centerY = 0.0f;
        _aoeEffects[i].radius = 0.0f;
        _aoeEffects[i].color = GEColor();
        _aoeEffects[i].remainingTime = 0.0f;
    }
}

void GEPlayer::bindWorldContext(const MapService* maps, EnemyService* enemies, ProjectileService* projectiles) {
    _mapsManager = maps;
    _enemyManager = enemies;
    _projectileManager = projectiles;
    _saveData = maps->getSaveData();

    int mapWorldWidth = _saveData->getMapTotalWidth();
    int mapWorldHeight = _saveData->getMapTotalHeight();

    _image.load("Src/Assets/Textures/player.png");
    setCenter(mapWorldWidth / 2.0f, mapWorldHeight / 2.0f);
    setMapBounds(mapWorldWidth, mapWorldHeight);
}

void GEPlayer::update(float deltaTime, Window& window) {
    updateCharacterState(deltaTime);

    float dirX = 0.0f;
    float dirY = 0.0f;

    if (window.keyPressed('W')) dirY -= 1.0f;
    if (window.keyPressed('S')) dirY += 1.0f;
    if (window.keyPressed('A')) dirX -= 1.0f;
    if (window.keyPressed('D')) dirX += 1.0f;

    if (window.keyPressed('Q')) _aoeKeyHeld = true;

    moveUpdate(deltaTime, dirX, dirY);

    autoAttack(deltaTime);
    aoeAttack(deltaTime);

    applyEnvironmentalEffects(deltaTime);
}

bool GEPlayer::collidesWithTileType(float newX, float newY, const MapService& maps, GECollisionType targetType) const {
    if (!_saveData) return false;

    int tileW = _saveData->getTileWidth();
    int tileH = _saveData->getTileHeight();
    int mapCols = _saveData->getMapColCount();
    int mapRows = _saveData->getMapRowCount();

    float playerCenterX = newX;
    float playerCenterY = newY;

    float playerHalfW = _image.width / 2.0f;
    float playerHalfH = _image.height / 2.0f;

    int centerTileX = static_cast<int>(playerCenterX / tileW);
    int centerTileY = static_cast<int>(playerCenterY / tileH);

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int tileX = centerTileX + dx;
            int tileY = centerTileY + dy;
            if (tileX < 0 || tileX >= mapCols || tileY < 0 || tileY >= mapRows)
                continue;

            int tileID = _saveData->getTileID(0, tileY, tileX);
            GETile* tile = maps.getTile(tileID);
            if (!tile) continue;

            if (tile->getCollisionType() != targetType)
                continue;

            float tileCenterX = tileX * tileW + tileW / 2.0f;
            float tileCenterY = tileY * tileH + tileH / 2.0f;
            tile->setCenter(tileCenterX, tileCenterY);

            float dxCenter = std::abs(playerCenterX - tileCenterX);
            float dyCenter = std::abs(playerCenterY - tileCenterY);

            float combinedHalfW = playerHalfW + tileW / 2.0f;
            float combinedHalfH = playerHalfH + tileH / 2.0f;

            if (dxCenter < combinedHalfW && dyCenter < combinedHalfH)
                return true;
        }
    }

    return false;
}

bool GEPlayer::collidesWithEnemies(float newX, float newY, const EnemyService& enemyManager) const {
    int enemyCount = enemyManager.getEnemyCount();
    for (int i = 0;i < enemyCount;i++) {
        GEEnemy* enemy = static_cast<GEEnemy*>(enemyManager.getEnemyAt(i));

        if (!enemy || !enemy->isAlive()) continue;

        if (this->collideAt(newX, newY, *enemy))
            return true;
    }
    return false;
}

bool GEPlayer::isBlockedAt(float x, float y) const {
    if (!_saveData) return false;

    if (collidesWithTileType(x, y, *_mapsManager, GECollisionType::Water))
        return true;

    if (collidesWithEnemies(x, y, *_enemyManager))
        return true;

    return false;
}

void GEPlayer::applyEnvironmentalEffects(float deltaTime) {
    static bool wasInFire = false;                // 上一帧是否在火焰中
    static float fireTimer = 0.0f;

    constexpr float FIRE_DAMAGE_INTERVAL = 1.0f;  // 连续灼烧间隔
    constexpr int FIRE_DAMAGE = 15;

    bool inFire = collidesWithTileType(getCenterX(), getCenterY(), *_mapsManager, GECollisionType::Fire);

    if (inFire) {
        // 第一次进入火焰 → 立即受伤
        if (!wasInFire) {
            takeDamage(FIRE_DAMAGE);
            fireTimer = 0.0f;
        }
        else {
            // 持续在火焰中 → 累积计时
            fireTimer += deltaTime;
            if (fireTimer >= FIRE_DAMAGE_INTERVAL) {
                takeDamage(FIRE_DAMAGE);
                fireTimer = 0.0f;
            }
        }
    }
    else {
        fireTimer = 0.0f;
    }

    wasInFire = inFire;
}

void GEPlayer::applyMovementBounds(float& newX, float& newY) {
    float camW = _saveData->getScreenWidth();
    float camH = _saveData->getScreenHeight();
    float mapW = _saveData->getMapTotalWidth();
    float mapH = _saveData->getMapTotalHeight();

    float minCenterX = camW / 2.0f;
    float maxCenterX = mapW - camW / 2.0f;
    float minCenterY = camH / 2.0f;
    float maxCenterY = mapH - camH / 2.0f;


    newX = clamp(newX, minCenterX, maxCenterX);
    newY = clamp(newY, minCenterY, maxCenterY);
}

void GEPlayer::autoAttack(float deltaTime) {
    if (!_enemyManager || !_projectileManager) {
        return;
    }

    _autoAttackTimer += deltaTime;

    float interval = _autoAttackIntervalBase / _autoAttackSpeedMultiplier;

    if (_autoAttackTimer < interval) {
        return;
    }

    _autoAttackTimer -= interval;

    GEEnemy* nearest = nullptr;
    float nearestDistSq = FLT_MAX;

    int enemyCount = _enemyManager->getEnemyCount();
    for (int i = 0;i < enemyCount;i++) {
        GEEnemy* enemy = static_cast<GEEnemy*>(_enemyManager->getEnemyAt(i));
        if (!enemy || !enemy->isAlive()) continue;
        float dx = enemy->getCenterX() - getCenterX();
        float dy = enemy->getCenterY() - getCenterY();
        float distSq = dx * dx + dy * dy;
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = enemy;
        }
    }

    if (!nearest) {
        return;
    }

    float playerCenterX = getCenterX();
    float playerCenterY = getCenterY();
    float targetCenterX = nearest->getCenterX();
    float targetCenterY = nearest->getCenterY();
    float dirX = targetCenterX - playerCenterX;
    float dirY = targetCenterY - playerCenterY;
    float len = std::sqrt(dirX * dirX + dirY * dirY);
    if (len == 0.0f) {
        return;
    }
    dirX /= len;
    dirY /= len;

    _projectileManager->addProjectile(ProjectileOwner::FromPlayer, playerCenterX, playerCenterY, dirX, dirY, PLAYER_PROJECTILE_SPEED, PLAYER_PROJECTILE_DAMAGE);
}

void GEPlayer::aoeAttack(float deltaTime) {
    updateAoeEffects(deltaTime);

    // calculate cooldonw
    if (_aoeCooldownTimer > 0.0f) {
        _aoeCooldownTimer = max(_aoeCooldownTimer - deltaTime, 0.0f);
    }

    // check user input
    if (_aoeKeyHeld && _aoeCooldownTimer <= 0.0f) {
        executeAoeSkill();
    }
    _aoeKeyHeld = false;
}

void GEPlayer::executeAoeSkill() {
    if (!_enemyManager) return;

    const float originX = getCenterX();
    const float originY = getCenterY();

    // get enemies near the player
    GEEnemy* nearby[PLAYER_MAX_AOE_TARGETS];
    int nearbyCount = findEnemiesWithinRadius(originX, originY, _aoeRadius, nearby, PLAYER_MAX_AOE_TARGETS);
    if (nearbyCount <= 0) return;

    // select top n HP enemies
    GEEnemy* topTargets[PLAYER_MAX_AOE_TARGETS];
    int topCount = selectTopEnemiesByHP(nearby, nearbyCount, _aoeTargetCount, topTargets);
    if (topCount <= 0) return;

    // show range of aoe
    spawnAoeEffect(originX, originY, _aoeRadius, GEColor(0, 0, 255));

    // show hit vfx of aoe
    const float IMPACT_RADIUS = 30.0f;

    for (int i = 0;i < topCount;++i) {
        GEEnemy* enemy = topTargets[i];
        enemy->takeDamage(_aoeDamage);
        if (!enemy->isAlive() && _enemyManager) {
            _enemyManager->registerEnemyKill(enemy->getType());
        }
        spawnAoeEffect(enemy->getCenterX(), enemy->getCenterY(), IMPACT_RADIUS, GEColor(255, 0, 0));
    }

    // reset skill cooldown time
    _aoeCooldownTimer = _aoeCooldown;
}

int GEPlayer::findEnemiesWithinRadius(float cx, float cy, float radius, GEEnemy** outList, int maxCount) const {
    if (!_enemyManager) return 0;

    const float radiusSq = radius * radius;
    int count = 0;
    const int enemyCount = _enemyManager->getEnemyCount();

    for (int i = 0;i < enemyCount;++i) {
        GEEnemy* e = static_cast<GEEnemy*>(_enemyManager->getEnemyAt(i));
        if (!e || !e->isAlive()) continue;

        float dx = e->getCenterX() - cx;
        float dy = e->getCenterY() - cy;
        if (dx * dx + dy * dy <= radiusSq) {
            if (count < maxCount) {
                outList[count++] = e;
            }
        }
    }
    return count;
}

int GEPlayer::selectTopEnemiesByHP(GEEnemy** input, int count, int topN, GEEnemy** output) const {
    if (count == 0 || topN <= 0) return 0;

    // Sort
    for (int i = 0;i < count - 1;++i) {
        for (int j = i + 1;j < count;++j) {
            if (input[j]->getHP() > input[i]->getHP()) {
                std::swap(input[i], input[j]);
            }
        }
    }

    const int resultCount = min(topN, count);
    for (int i = 0;i < resultCount;++i) {
        output[i] = input[i];
    }
    return resultCount;
}

void GEPlayer::updateAoeEffects(float deltaTime) {
    for (int i = 0;i < PLAYER_MAX_AOE_EFFECTS;i++) {
        if (!_aoeEffects[i].active) continue;
        _aoeEffects[i].remainingTime -= deltaTime;
        if (_aoeEffects[i].remainingTime <= 0.0f) {
            _aoeEffects[i].active = false;
            _aoeEffects[i].remainingTime = 0.0f;
        }
    }
}

void GEPlayer::spawnAoeEffect(float centerX, float centerY, float radius, GEColor color) {
    int slot = -1;
    for (int i = 0;i < PLAYER_MAX_AOE_EFFECTS;i++) {
        if (!_aoeEffects[i].active) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        float minTime = _aoeEffects[0].remainingTime;
        slot = 0;
        for (int i = 1;i < PLAYER_MAX_AOE_EFFECTS;i++) {
            if (_aoeEffects[i].remainingTime < minTime) {
                minTime = _aoeEffects[i].remainingTime;
                slot = i;
            }
        }
    }

    _aoeEffects[slot].active = true;
    _aoeEffects[slot].centerX = centerX;
    _aoeEffects[slot].centerY = centerY;
    _aoeEffects[slot].radius = radius;
    _aoeEffects[slot].color = color;
    _aoeEffects[slot].remainingTime = _aoeEffectDuration;
}

void GEPlayer::drawAoeIndicatorIfNeeded(Window& window, const GECamera& camera) const {
    if (!GEDebug::shared().needDrawAOEIndicators()) return;

    const bool ready = _aoeCooldownTimer <= 0.0f;
    if (ready) {
        drawCircle(window, camera, getCenterX(), getCenterY(), _aoeRadius, GEColor(0, 200, 255));
    }
    else {
        drawCircle(window, camera, getCenterX(), getCenterY(), _aoeRadius, GEColor(90, 90, 90));
    }
}

void GEPlayer::drawAoeEffects(Window& window, const GECamera& camera) const {
    for (int i = 0;i < PLAYER_MAX_AOE_EFFECTS;i++) {
        if (!_aoeEffects[i].active) continue;
        drawCircle(window, camera, _aoeEffects[i].centerX, _aoeEffects[i].centerY, _aoeEffects[i].radius, _aoeEffects[i].color);
    }
}

void GEPlayer::drawCircle(Window& window, const GECamera& camera, float centerX, float centerY, float radius, GEColor color) const {
    int camX = camera.getX();
    int camY = camera.getY();

    int winW = window.getWidth();
    int winH = window.getHeight();

    int radiusInt = static_cast<int>(radius);
    if (radiusInt <= 0) {
        return;
    }
    int cx = static_cast<int>(centerX - camX);
    int cy = static_cast<int>(centerY - camY);
    int radiusSq = radiusInt * radiusInt;

    for (int dx = -radiusInt;dx <= radiusInt;++dx) {
        int rem = radiusSq - dx * dx;
        if (rem < 0) continue;
        int dy = static_cast<int>(std::sqrt(static_cast<float>(rem)));

        int x = cx + dx;
        if (x < 0 || x >= winW) continue;

        int y1 = cy + dy;
        int y2 = cy - dy;
        if (y1 >= 0 && y1 < winH) window.draw(x, y1, color.r, color.g, color.b);
        if (y2 >= 0 && y2 < winH) window.draw(x, y2, color.r, color.g, color.b);
    }
}

void GEPlayer::onEvent(const GEEvent& event)
{
}

void GEPlayer::draw(Window& window, const GECamera& camera) const {
    GECharacter::draw(window, camera);
    drawAoeIndicatorIfNeeded(window, camera);
    drawAoeEffects(window, camera);
}

void GEPlayer::applyPowerUp(GEPowerUpType type) {
    switch (type) {
    case GEPowerUpType::AttackSpeedBoost:
        _autoAttackSpeedMultiplier = min(PLAYER_MAX_AUTO_ATTACK_SPEED_MULTIPLIER,
            _autoAttackSpeedMultiplier + 0.35f);
        break;
    case GEPowerUpType::AdditionalAoeTarget:
        _aoeTargetCount = min(PLAYER_MAX_AOE_TARGETS, _aoeTargetCount + 1);
        break;
    }
}

void GEPlayer::takeDamage(int value) {
    if (value <= 0) return;
    GECharacter::takeDamage(value);
    triggerDamageFlash(GEColor(255, 0, 0), 0.25f);
}