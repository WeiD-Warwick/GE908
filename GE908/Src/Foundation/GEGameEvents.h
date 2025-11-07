#pragma once

class Character;
class GECollisible;
class GEEnemy;
class GEPlayer;
class GEPowerUp;

enum class GEEnemyType;
enum class GEPowerUpType;

struct GECharacterDamagedEvent {
    Character* target = nullptr;
    GECollisible* source = nullptr;
    int amount = 0;
};

struct GEEnemyKilledEvent {
    GEEnemy* enemy = nullptr;
    GEEnemyType type;
};

struct GEPowerUpCollectedEvent {
    GEPlayer* player = nullptr;
    GEPowerUp* powerUp = nullptr;
    GEPowerUpType type;
};