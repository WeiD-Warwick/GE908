#include "Src/GameManager.h"
#include "Src/Core/GEPowerUpManager.h"
#include "Src/Core/GEProjectileManager.h"
#include "Src/Core/GEEnemyManager.h"

int main() {
    Window window;
    window.create(854, 480, "WM908", false);
    GEPlayer player;
    GEEnemyManager enemyManager; 
    GEProjectileManager projectileManager;
    GEPowerUpManager powerUpManager;
    GEMapsManager maps;

    GameManager game(window, maps, player, enemyManager, projectileManager, powerUpManager);
    game.run();
    return 0;
}