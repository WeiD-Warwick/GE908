#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GESaveData.h"
#include "../../Foundation/GEWindow.h"
#include "GETile.h"

using namespace GamesEngineeringBase;

class GEMapsManager {
private:

    GETile** _tiles = nullptr;                  // image cache
    int _tileCount = 0;
    GESaveData* _saveData = nullptr;                // save data

    // load all tiles images from folder
    void loadTileResources(const std::string& folderPath);

    // load save data from file
    void loadSaveData(const std::string& filePath);

public:

    GEMapsManager();
    ~GEMapsManager();

    void load(const std::string& tilesFolderPath, const std::string& saveDatafilePath);

    // get tile Image
    GETile* getTile(int tileID) const;

    // get current save data
    GESaveData* getSaveData() { return _saveData; }

    void draw(GEWindow& window, GECamera& camera);
};

