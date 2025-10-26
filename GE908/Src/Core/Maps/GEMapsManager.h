//
// Created by W.D. on 26/10/25
//

#pragma once
#include "../GESaveData.h"
#include "../../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class GEMapsManager {
private:

    Image** _tileImages = nullptr;                  // image cache
    int _tileCount = 0;
    GESaveData* _saveData = nullptr;                // save data

public:
    GEMapsManager();
    ~GEMapsManager();

    // load all tiles images from folder
    bool loadTileResources(const std::string& folderPath);

    // load save data from file
    void loadSaveData(const std::string& filePath);

    // get tile Image
    Image* getTileImage(int tileID) const;

    // get current save data
    const GESaveData* getSaveData() const { return _saveData; }
};

