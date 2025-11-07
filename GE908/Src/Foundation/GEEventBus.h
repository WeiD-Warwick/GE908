#pragma once
#include "GEGameEvents.h"

// Synchronous event hub specialised for the limited set of game events.
// Avoids dynamic containers by keeping fixed-size listener slots per event type.
class GEEventBus {
public:
    typedef void (*CharacterDamagedCallback)(void*, const GECharacterDamagedEvent&);
    typedef void (*EnemyKilledCallback)(void*, const GEEnemyKilledEvent&);
    typedef void (*PowerUpCollectedCallback)(void*, const GEPowerUpCollectedEvent&);

    GEEventBus();

    void subscribeToCharacterDamaged(void* context, CharacterDamagedCallback callback);
    void subscribeToEnemyKilled(void* context, EnemyKilledCallback callback);
    void subscribeToPowerUpCollected(void* context, PowerUpCollectedCallback callback);

    void publishCharacterDamaged(const GECharacterDamagedEvent& event) const;
    void publishEnemyKilled(const GEEnemyKilledEvent& event) const;
    void publishPowerUpCollected(const GEPowerUpCollectedEvent& event) const;

private:
    static constexpr int MAX_CHARACTER_DAMAGED_LISTENERS = 6;
    static constexpr int MAX_ENEMY_KILLED_LISTENERS = 8;
    static constexpr int MAX_POWERUP_COLLECTED_LISTENERS = 6;

    struct CharacterDamagedListener {
        void* context;
        CharacterDamagedCallback callback;
    };

    struct EnemyKilledListener {
        void* context;
        EnemyKilledCallback callback;
    };

    struct PowerUpCollectedListener {
        void* context;
        PowerUpCollectedCallback callback;
    };

    CharacterDamagedListener _characterDamagedListeners[MAX_CHARACTER_DAMAGED_LISTENERS];
    EnemyKilledListener _enemyKilledListeners[MAX_ENEMY_KILLED_LISTENERS];
    PowerUpCollectedListener _powerUpCollectedListeners[MAX_POWERUP_COLLECTED_LISTENERS];

    int _characterDamagedCount;
    int _enemyKilledCount;
    int _powerUpCollectedCount;
};

inline GEEventBus::GEEventBus()
    : _characterDamagedCount(0),
    _enemyKilledCount(0),
    _powerUpCollectedCount(0) {
    for (int i = 0; i < MAX_CHARACTER_DAMAGED_LISTENERS; ++i) {
        _characterDamagedListeners[i].context = nullptr;
        _characterDamagedListeners[i].callback = nullptr;
    }
    for (int i = 0; i < MAX_ENEMY_KILLED_LISTENERS; ++i) {
        _enemyKilledListeners[i].context = nullptr;
        _enemyKilledListeners[i].callback = nullptr;
    }
    for (int i = 0; i < MAX_POWERUP_COLLECTED_LISTENERS; ++i) {
        _powerUpCollectedListeners[i].context = nullptr;
        _powerUpCollectedListeners[i].callback = nullptr;
    }
}

inline void GEEventBus::subscribeToCharacterDamaged(void* context, CharacterDamagedCallback callback) {
    if (_characterDamagedCount >= MAX_CHARACTER_DAMAGED_LISTENERS || callback == nullptr) {
        return;
    }

    CharacterDamagedListener& listener = _characterDamagedListeners[_characterDamagedCount++];
    listener.context = context;
    listener.callback = callback;
}

inline void GEEventBus::subscribeToEnemyKilled(void* context, EnemyKilledCallback callback) {
    if (_enemyKilledCount >= MAX_ENEMY_KILLED_LISTENERS || callback == nullptr) {
        return;
    }

    EnemyKilledListener& listener = _enemyKilledListeners[_enemyKilledCount++];
    listener.context = context;
    listener.callback = callback;
}

inline void GEEventBus::subscribeToPowerUpCollected(void* context, PowerUpCollectedCallback callback) {
    if (_powerUpCollectedCount >= MAX_POWERUP_COLLECTED_LISTENERS || callback == nullptr) {
        return;
    }

    PowerUpCollectedListener& listener = _powerUpCollectedListeners[_powerUpCollectedCount++];
    listener.context = context;
    listener.callback = callback;
}

inline void GEEventBus::publishCharacterDamaged(const GECharacterDamagedEvent& event) const {
    for (int i = 0; i < _characterDamagedCount; ++i) {
        const CharacterDamagedListener& listener = _characterDamagedListeners[i];
        if (listener.callback) {
            listener.callback(listener.context, event);
        }
    }
}

inline void GEEventBus::publishEnemyKilled(const GEEnemyKilledEvent& event) const {
    for (int i = 0; i < _enemyKilledCount; ++i) {
        const EnemyKilledListener& listener = _enemyKilledListeners[i];
        if (listener.callback) {
            listener.callback(listener.context, event);
        }
    }
}

inline void GEEventBus::publishPowerUpCollected(const GEPowerUpCollectedEvent& event) const {
    for (int i = 0; i < _powerUpCollectedCount; ++i) {
        const PowerUpCollectedListener& listener = _powerUpCollectedListeners[i];
        if (listener.callback) {
            listener.callback(listener.context, event);
        }
    }
}