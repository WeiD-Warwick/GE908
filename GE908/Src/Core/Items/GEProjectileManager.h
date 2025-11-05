#pragma once
#include "GEProjectile.h"
#include "../../Core/Character/GEPlayer.h"
#include "../../Core/Character/GEEnemyManager.h"

#define MAX_PROJECTILES 5000

class GEProjectileManager {

private:

    GEProjectile* _projectiles[MAX_PROJECTILES];
   

public:
    GEProjectileManager();
    ~GEProjectileManager();

    void addProjectile(ProjectileOwner from, float startPointX, float startPointY,
        float dirX, float dirY, float speed, int damage);

    void update(float deltaTime, GEEnemyManager& enemyManager, GEPlayer& player);
    void draw(GEWindow& window, const GECamera& camera);
};