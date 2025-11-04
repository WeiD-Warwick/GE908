#include "GEProjectileManager.h"
#define MAX_PROJECTILES 200

GEProjectileManager::GEProjectileManager() {
	for (int i = 0; i < MAX_PROJECTILES; i++) {
		_projectiles[i] = nullptr;
	}
}

GEProjectileManager::~GEProjectileManager() {
	for (int i = 0; i < MAX_PROJECTILES; i++) {
		if (_projectiles[i]) {
			delete _projectiles[i];
		}
	}
}

void GEProjectileManager::addProjectile(ProjectileOwner from, int x, int y, float dirX, float dirY, float speed, int damage) {

    if (_count < 0 || _count >= MAX_PROJECTILES) {
        GELog::shared().error("!!! ProjectileManager overflow: count: " + std::to_string(_count));
        return;
    }

	const std::string& filePath = (from == FromPlayer) ? "Src/Assets/Textures/arrow.png" : "Src/Assets/Textures/enemy_bullet.png";

	GEProjectile* projectile = new GEProjectile(filePath, from, x, y, dirX, dirY, speed, damage);

	_projectiles[_count++] = projectile;
}

void GEProjectileManager::update(float deltaTime, GEEnemyManager& enemyManager, GEPlayer& player) {

    for (int i = 0; i < _count; ) {
        GEProjectile* projectile = _projectiles[i];
        if (!projectile) {
            _projectiles[i] = _projectiles[_count - 1];
            _projectiles[_count - 1] = nullptr;
            _count--;
            continue;
        }

        if (projectile && !projectile->isActive()) {
            delete projectile;
            _projectiles[i] = _projectiles[_count - 1];
            _projectiles[_count - 1] = nullptr;
            _count--;
            continue;
        }

        projectile->update(deltaTime);

        if (projectile->getOwner() == FromPlayer) {
            int enemyCount = enemyManager.getEnemyCount();
            for (int j = 0; j < enemyCount; j++) {
                GEEnemy* enemy = enemyManager.getEnemyAt(j);
                if (enemy && enemy->isAlive() && projectile->collide(*enemy)) {
                    enemy->takeDamage(projectile->getDamage());
                    projectile->deactivate();
                    break;
                }
            }
        }
        else {
            if (projectile->collide(player)) {
                player.takeDamage(projectile->getDamage());
                projectile->deactivate();
            }
        }

        i++;
    }
}


void GEProjectileManager::draw(GEWindow& window, const GECamera& camera) {
    if (_count <= 0) return;
    for (int i = 0; i < _count; i++) {
        GEProjectile* projectile = _projectiles[i];
        if (projectile && projectile->isActive())
            projectile->draw(window, camera);
    }
}
