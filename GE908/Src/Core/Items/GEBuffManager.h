#pragma once

#include "GEBuff.h"

class GEPlayer;
class GECamera;
class GESaveData;
class GEWindow;

class GEBuffManager {
private:
    static constexpr int MAX_BUFF_ITEMS = 6;

    GEBuff* _buffs[MAX_BUFF_ITEMS];
    GESaveData* _saveData = nullptr;
    float _spawnTimer = 0.0f;
    float _spawnInterval = 18.0f;

    void spawnBuff();

public:
    GEBuffManager();
    ~GEBuffManager();

    void load(GESaveData* saveData);
    void update(float deltaTime, GEPlayer& player);
    void draw(GEWindow& window, const GECamera& camera);
};