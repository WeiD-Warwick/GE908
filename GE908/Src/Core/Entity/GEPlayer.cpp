#include "GEPlayer.h"
#include "../SaveLoad/GESaveData.h"
#include "../../Foundation/GEDebug.h"
#include "GEEnemy.h"
#include <cmath>

static float fireTimer = 0.0f;

GEPlayer::GEPlayer()
    : GECharacter(Player::playerSpriteFilePath, GECollisionType::Player) {
    _hp = 300;
    _speed = 300;
    _maxHp = _hp;

    setContactDamageCooldownDuration(0.5f);
}

void GEPlayer::bind(GEContext& ctx) {
    _mapsManager = &ctx.mapProvider();
    _enemyManager = &ctx.enemyProvider();
    _projectileManager = &ctx.projectileProvider();
    _powerUpManager = &ctx.powerupProvider();

    if (_mapsManager) {
        _saveData = _mapsManager->getSaveData();
        if (_saveData) {
            _image.load(Player::playerSpriteFilePath);
            int chunkPixelW = _saveData->getChunkPixelWidth();
            int chunkPixelH = _saveData->getChunkPixelHeight();
            if (chunkPixelW <= 0) chunkPixelW = _image.width * 4;
            if (chunkPixelH <= 0) chunkPixelH = _image.height * 4;

            //setCenter(chunkPixelW / 2.0f, chunkPixelH / 2.0f);
            if (_saveData->isInfiniteMap())
                setMapBounds(-1, -1);
            else
                setMapBounds(chunkPixelW, chunkPixelH);

            const GEPlayerState* state = _saveData->getPlayerState();
            if (state) {
                applyState(*state);
            }
            else {
                setCenter(chunkPixelW / 2.0f, chunkPixelH / 2.0f);
                float boundedX = getCenterX();
                float boundedY = getCenterY();
                applyMovementBounds(boundedX, boundedY);
                setCenter(boundedX, boundedY);
            }

            _saveData->updateActiveChunkFromWorldPosition(getCenterX(), getCenterY());
        }
    }
}


void GEPlayer::update(float deltaTime, Window& window) {
    updateCharacterState(deltaTime);

    if (_attackSpeedBuffTimer > 0.0f) {
        _autoAttackSpeedMultiplier = Player::PLAYER_BUFF_ATTACK_SPEED_MULTIPLIER;
        _attackSpeedBuffTimer = max(0.0f, _attackSpeedBuffTimer - deltaTime);
        if (_attackSpeedBuffTimer <= 0.0f) {
            _autoAttackSpeedMultiplier = 1.0f;
        }
    }

    if (_aoeTargetBuffTimer > 0.0f) {
        _aoeTargetCount = Player::PLAYER_MAX_AOE_TARGETS;
        _aoeTargetBuffTimer = max(0.0f, _aoeTargetBuffTimer - deltaTime);
        if (_aoeTargetBuffTimer <= 0.0f) {
            _aoeTargetCount = Player::PLAYER_BASE_AOE_TARGETS;
        }
    }

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

bool GEPlayer::collidesWithTileType(float newX, float newY, const MapProvider& maps, GECollisionType targetType) const {
    if (!_saveData) return false;

    int tileW = _saveData->getTileWidth();
    int tileH = _saveData->getTileHeight();

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

            int tileID = _saveData->getTileID(tileY, tileX);
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

bool GEPlayer::collidesWithEnemies(float newX, float newY, const EnemyProvider& enemyManager) const {
    int enemyCount = enemyManager.getEnemyCount();
    for (int i = 0;i < enemyCount;i++) {
        GEEnemy* enemy = static_cast<GEEnemy*>(enemyManager.getEnemyAt(i));

        if (!enemy || !enemy->isAlive()) continue;

        const bool currentlyColliding = this->collide(*enemy);
        if (!this->collideAt(newX, newY, *enemy))
            continue;

        if (!currentlyColliding)
            return true;

        const float currentDx = getCenterX() - enemy->getCenterX();
        const float currentDy = getCenterY() - enemy->getCenterY();
        const float newDx = newX - enemy->getCenterX();
        const float newDy = newY - enemy->getCenterY();

        const float currentDistSq = currentDx * currentDx + currentDy * currentDy;
        const float newDistSq = newDx * newDx + newDy * newDy;

        if (newDistSq <= currentDistSq)
            return true;
    }
    return false;
}

bool GEPlayer::isBlockedAt(float x, float y) const {
    if (!_saveData) return false;

    if (collidesWithTileType(x, y, *_mapsManager, GECollisionType::Tile_Water))
        return true;

    if (collidesWithEnemies(x, y, *_enemyManager))
        return true;

    return false;
}

void GEPlayer::applyEnvironmentalEffects(float deltaTime) {

    static bool wasInFire = false;
    static float fireTimer = 0.0f;

    bool inFire = collidesWithTileType(getCenterX(), getCenterY(), *_mapsManager, GECollisionType::Tile_Fire);

    if (inFire) {
        if (!wasInFire) {
            takeDamage(Player::FIRE_DAMAGE);
            fireTimer = 0.0f;
        }
        else {
            fireTimer += deltaTime;
            if (fireTimer >= Player::FIRE_DAMAGE_INTERVAL) {
                takeDamage(Player::FIRE_DAMAGE);
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
    if (!_saveData || _saveData->isInfiniteMap() || _mapWidth <= 0 || _mapHeight <= 0) return;

    float camW = static_cast<float>(_saveData->getScreenWidth());
    float camH = static_cast<float>(_saveData->getScreenHeight());

    float mapW = static_cast<float>(_mapWidth);
    float mapH = static_cast<float>(_mapHeight);

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

    _projectileManager->addProjectile(ProjectileOwner::FromPlayer, 
                                      playerCenterX, playerCenterY, 
                                      dirX, dirY, 
                                      Player::PLAYER_PROJECTILE_SPEED,
                                      Player::PLAYER_PROJECTILE_DAMAGE);
}

void GEPlayer::aoeAttack(float deltaTime) {
    updateAoeImpacts(deltaTime);

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

    GEEnemy* topEnemies[Player::PLAYER_MAX_AOE_TARGETS];
    int topEnemyHp[Player::PLAYER_MAX_AOE_TARGETS];
    int candidateCount = 0;

    const int enemyCount = _enemyManager->getEnemyCount();
    const float radiusSq = _aoeRadius * _aoeRadius;
    for (int i = 0; i < enemyCount; ++i) {
        GEEnemy* enemy = static_cast<GEEnemy*>(_enemyManager->getEnemyAt(i));
        if (!enemy || !enemy->isAlive()) {
            continue;
        }

        const float dx = enemy->getCenterX() - originX;
        const float dy = enemy->getCenterY() - originY;
        if (dx * dx + dy * dy > radiusSq) {
            continue;
        }

        const int hp = enemy->getHP();
        int insertPos = 0;
        while (insertPos < candidateCount && hp <= topEnemyHp[insertPos]) {
            ++insertPos;
        }

        if (candidateCount < Player::PLAYER_MAX_AOE_TARGETS) {
            for (int j = candidateCount; j > insertPos; --j) {
                topEnemies[j] = topEnemies[j - 1];
                topEnemyHp[j] = topEnemyHp[j - 1];
            }
            topEnemies[insertPos] = enemy;
            topEnemyHp[insertPos] = hp;
            ++candidateCount;
        }
        else if (insertPos < Player::PLAYER_MAX_AOE_TARGETS) {
            for (int j = Player::PLAYER_MAX_AOE_TARGETS - 1; j > insertPos; --j) {
                topEnemies[j] = topEnemies[j - 1];
                topEnemyHp[j] = topEnemyHp[j - 1];
            }
            topEnemies[insertPos] = enemy;
            topEnemyHp[insertPos] = hp;
        }
    }

    if (candidateCount == 0) {
        return;
    }

    const int targetsToHit = min(candidateCount, _aoeTargetCount);

    for (int i = 0; i < targetsToHit; ++i) {
        GEEnemy& enemy = *topEnemies[i];

        enemy.takeDamage(Player::PLAYER_AOE_DAMAGE);
        if (!enemy.isAlive()) {
            _enemyManager->registerEnemyKill(enemy.getType());
            if (_powerUpManager) {
                _powerUpManager->onEnemyDefeated(GEPoint(enemy.getCenterX(), enemy.getCenterY()));
            }
        }
        recordAoeImpact(enemy.getCenterX(), enemy.getCenterY());
    }
    _aoeCooldownTimer = _aoeCooldown;
}

void GEPlayer::updateAoeImpacts(float deltaTime) {
    int index = 0;
    while (index < _aoeImpactCount) {
        _aoeImpacts[index].remainingTime -= deltaTime;
        if (_aoeImpacts[index].remainingTime > 0.0f) {
            ++index;
            continue;
        }
        for (int j = index; j < _aoeImpactCount - 1; ++j) {
            _aoeImpacts[j] = _aoeImpacts[j + 1];
        }
        --_aoeImpactCount;
    }
}

void GEPlayer::recordAoeImpact(float centerX, float centerY) {
    if (_aoeImpactCount >= Player::PLAYER_MAX_AOE_EFFECTS) {
        for (int i = 1; i < _aoeImpactCount; ++i) {
            _aoeImpacts[i - 1] = _aoeImpacts[i];
        }
        --_aoeImpactCount;
    }
    AoeImpact& impact = _aoeImpacts[_aoeImpactCount++];
    impact.x = centerX;
    impact.y = centerY;
    impact.remainingTime = _aoeEffectDuration;
}


void GEPlayer::drawAoeImpacts(Window& window, const GECamera& camera) const {
    const float impactRadius = 30.0f;
    for (int i = 0; i < _aoeImpactCount; ++i) {
        const AoeImpact& impact = _aoeImpacts[i];
        if (impact.remainingTime > 0.0f) {
            drawImpact(window, camera, impact.x, impact.y, impactRadius, GREEN);
        }
    }
}

void GEPlayer::drawImpact(Window& window, const GECamera& camera, float centerX, float centerY, float radius, GEColor color) const {
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

void GEPlayer::draw(Window& window, const GECamera& camera) const {
    GECharacter::draw(window, camera);
    drawAoeImpacts(window, camera);
}

void GEPlayer::applyPowerUp(GEPowerUpType type) {
    switch (type) {
    case GEPowerUpType::AttackSpeedBoost:
        _autoAttackSpeedMultiplier = Player::PLAYER_BUFF_ATTACK_SPEED_MULTIPLIER;
        _attackSpeedBuffTimer = Player::PLAYER_POWERUP_DURATION_SECONDS;
        break;
    case GEPowerUpType::AdditionalAoeTarget:
        _aoeTargetCount = Player::PLAYER_MAX_AOE_TARGETS;
        _aoeTargetBuffTimer = Player::PLAYER_POWERUP_DURATION_SECONDS;
        break;
    case GEPowerUpType::HealPlayer:
        heal(Player::PLAYER_HEAL_VALUE);
        break;
    }
}

void GEPlayer::takeDamage(int value) {
    if (value <= 0) return;
    GECharacter::takeDamage(value);
    triggerDamageFlash(GEColor(255, 0, 0), 0.25f);
}

GEPlayerState GEPlayer::snapshotState() const {
    GEPlayerState state;
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.hp = _hp;
    state.maxHp = _maxHp;
    state.speed = _speed;
    state.autoAttackTimer = _autoAttackTimer;
    state.autoAttackSpeedMultiplier = _autoAttackSpeedMultiplier;
    state.attackSpeedBuffTimer = _attackSpeedBuffTimer;
    state.aoeCooldownTimer = _aoeCooldownTimer;
    state.aoeCooldown = _aoeCooldown;
    state.contactDamageCooldownTimer = _contactDamageCooldownTimer;
    state.aoeTargetCount = _aoeTargetCount;
    state.aoeKeyHeld = _aoeKeyHeld;
    state.aoeTargetBuffTimer = _aoeTargetBuffTimer;
    return state;
}

void GEPlayer::applyState(const GEPlayerState& state) {
    _speed = state.speed > 0 ? state.speed : _speed;
    if (state.maxHp > 0) setMaxHP(state.maxHp);
    setCurrentHP(state.hp);
    _autoAttackTimer = state.autoAttackTimer;
    _autoAttackSpeedMultiplier = max(0.1f, state.autoAttackSpeedMultiplier);
    _attackSpeedBuffTimer = max(0.0f, state.attackSpeedBuffTimer);
    _aoeCooldownTimer = state.aoeCooldownTimer;
    if (state.aoeCooldown > 0.0f) _aoeCooldown = state.aoeCooldown;
    _contactDamageCooldownTimer = max(0.0f, state.contactDamageCooldownTimer);
    if (state.aoeTargetCount > 0) {
        _aoeTargetCount = min(state.aoeTargetCount, Player::PLAYER_MAX_AOE_TARGETS);
    }
    else {
        _aoeTargetCount = Player::PLAYER_BASE_AOE_TARGETS;
    }
    _aoeTargetBuffTimer = max(0.0f, state.aoeTargetBuffTimer);
    _aoeKeyHeld = state.aoeKeyHeld;

    if (_attackSpeedBuffTimer > 0.0f) {
        _autoAttackSpeedMultiplier = Player::PLAYER_BUFF_ATTACK_SPEED_MULTIPLIER;
    }
    else {
        _autoAttackSpeedMultiplier = 1.0f;
    }

    if (_aoeTargetBuffTimer > 0.0f) {
        _aoeTargetCount = Player::PLAYER_MAX_AOE_TARGETS;
    }
    else {
        _aoeTargetCount = Player::PLAYER_BASE_AOE_TARGETS;
    }

    float newX = state.centerX;
    float newY = state.centerY;
    applyMovementBounds(newX, newY);
    setCenter(newX, newY);
}