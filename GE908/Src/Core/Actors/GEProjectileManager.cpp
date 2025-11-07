#include "GEProjectileManager.h"

GEProjectileManager::GEProjectileManager() {
	for (int i = 0;i < MAX_PROJECTILES;i++) {
		_projectiles[i] = nullptr;
	}
}

GEProjectileManager::~GEProjectileManager() {
    for (int i = 0;i < MAX_PROJECTILES;i++) {
        delete _projectiles[i];
    }
}

void GEProjectileManager::addProjectile(ProjectileOwner from, float startPointX, float startPointY, float dirX, float dirY, float speed, int damage) {
    for (int i = 0;i < MAX_PROJECTILES;i++) {
        GEProjectile* projectile = _projectiles[i];
        // find a destroy place and replace it
        if (!projectile || !projectile->isActive()) {
            delete projectile;
            const std::string& filePath = (from == ProjectileOwner::FromPlayer) ? "Src/Assets/Textures/arrow.png" : "Src/Assets/Textures/enemy_bullet.png";
            _projectiles[i] = new GEProjectile(filePath, from, startPointX, startPointY, dirX, dirY, speed, damage);
            return;
        }
    }
}

void GEProjectileManager::update(float deltaTime, GEEnemyManager& enemyManager, GEPlayer& player) {
    for (int i = 0;i < MAX_PROJECTILES;i++) {
        GEProjectile* projectile = _projectiles[i];

        if (!projectile || !projectile->isActive()) continue;

        projectile->update(deltaTime);

        if (projectile->getOwner() == ProjectileOwner::FromPlayer) {
            int n = enemyManager.getEnemyCount();
            for (int j = 0;j < n;j++) {
                GEEnemy* enemy = enemyManager.getEnemyAt(j);
                if (enemy && enemy->isAlive() && projectile->collide(*enemy)) {
                    enemy->takeDamage(projectile->getDamage());
                    projectile->deactivate();
                    break;
                }
            }
        } else {
            if (projectile->collide(player)) {
                player.takeDamage(projectile->getDamage());
                projectile->deactivate();
            }
        }
    }
}

void GEProjectileManager::draw(Window& window, const GECamera& camera) {
    for (int i = 0;i < MAX_PROJECTILES;i++) {
        GEProjectile* projectile = _projectiles[i];
        if (projectile && projectile->isActive()) {
            projectile->draw(window, camera);
        }
    }
}
