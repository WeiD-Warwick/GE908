#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GESaveData.h"

using namespace GamesEngineeringBase;

class GEMapsManager {
private:

    Image** _tileImages = nullptr;                  // image cache
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
    Image* getTileImage(int tileID) const;

    // get current save data
    GESaveData* getSaveData() { return _saveData; }
};

