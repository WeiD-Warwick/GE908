#include "GEProjectileManager.h"

GEProjectileManager::GEProjectileManager() {
    _projectiles.resize(Projectile::MAX_PROJECTILES);
    _projectiles.fillNull(Projectile::MAX_PROJECTILES);
}

GEProjectileManager::~GEProjectileManager() {
    _projectiles.destroyAll();
    _projectiles.clear();
}

void GEProjectileManager::addProjectile(ProjectileOwner from,
    float startPointX, float startPointY,
    float dirX, float dirY, float speed, int damage)
{
    const std::string texturePath =
        (from == ProjectileOwner::FromPlayer)
        ? Projectile::PLAYER_PROJECTILE_TEXTURE
        : Projectile::ENEMY_PROJECTILE_TEXTURE;

    // reuse inactive projectile slot if available
    for (unsigned int i = 0; i < _projectiles.size(); ++i) {
        GEProjectile* p = _projectiles[i];
        if (p == nullptr) {
            _projectiles[i] = new GEProjectile(texturePath, from,
                startPointX, startPointY, dirX, dirY, speed, damage);
            return;
        }
        if (!p->isActiveElement()) {
            p->spawn(texturePath, from,
                startPointX, startPointY, dirX, dirY, speed, damage);
            return;
        }
    }
}

void GEProjectileManager::update(float deltaTime, GEContext& ctx) {
    GEPlayer& player = static_cast<GEPlayer&>(ctx.playerProvider());

    _projectiles.forEachActive([&](GEProjectile* projectile, unsigned int) {
        if (!projectile->isActiveElement()) return;

        projectile->update(deltaTime);

        if (projectile->getOwner() == ProjectileOwner::FromPlayer) {
            // player bullets hit enemies
            int n = ctx.enemyProvider().getEnemyCount();
            for (int j = 0; j < n; ++j) {
                GEEnemy* enemy = static_cast<GEEnemy*>(ctx.enemyProvider().getEnemyAt(j));
                if (enemy && enemy->isAlive() && projectile->collide(*enemy)) {
                    enemy->takeDamage(projectile->getDamage());
                    if (!enemy->isAlive()) {
                        ctx.enemyProvider().registerEnemyKill(enemy->getType());
                        ctx.powerupProvider().onEnemyDefeated(
                            GEPoint(enemy->getCenterX(), enemy->getCenterY()));
                    }
                    projectile->deactivate();
                    break;
                }
            }
        }
        else {
            // enemy bullets hit player
            if (projectile->collide(player.collisionBody())) {
                player.takeDamage(projectile->getDamage());
                projectile->deactivate();
            }
        }
        });

    _projectiles.destroyInactive();
}

void GEProjectileManager::draw(Window& window, const GECamera& camera) {
    _projectiles.forEachActive([&](GEProjectile* projectile, unsigned int) {
        if (projectile && projectile->isActiveElement())
            projectile->draw(window, camera);
        });
}
