#pragma once
#include <string>
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEConst.h"
#include "../SaveLoad/GESaveData.h"
#include "../Interface/GEProvider.h"
#include "../Entity/GETile.h"

using namespace GamesEngineeringBase;

class GEMapsManager : public MapProvider {
private:
    GETile** _tiles = nullptr;// image cache
    int _tileCount = 0;
    GESaveData* _saveData = nullptr;// save data
public:

    GEMapsManager();
    ~GEMapsManager();

    // load all tiles images from folder
    void loadTileResources(const std::string& folderPath);

    // load save data from file
    void loadSaveData(const std::string& filePath);

    // get tile Image
    GETile* getTile(int tileID) const;

    // get current save data
    GESaveData* getSaveData() const { return _saveData;}

    void draw(Window& window, const GECamera& camera) const;
};

