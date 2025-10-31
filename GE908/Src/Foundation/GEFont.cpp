#include "GEFont.h"

Image* GEFont::fontChar[95] = { nullptr };
bool GEFont::isLoaded = false;

bool GEFont::load() {
    for (int i = 0; i < 95; ++i) {
        std::string filename = "Src/Assets/Fonts/" + std::to_string(i + 32) + ".png";

        Image* img = new Image();

        if (!img->load(filename)) {
            GELog::shared().error("Failed to load font character: " + filename);
            delete img; 
            for (int j = 0; j < i; ++j) {
                delete fontChar[j];
                fontChar[j] = nullptr;
            }
            return false;
        }

        fontChar[i] = img;
    }
	isLoaded = true;
    GELog::shared().info("All font characters loaded successfully.");
    return true;
}

Image* GEFont::getChar(char c) {
    if (!isLoaded) {
        GELog::shared().warning("Font not loaded! Call loadFont first.");
        return nullptr;
    }

    if (c < 32 || c > 126) return nullptr;
    return fontChar[c - 32];
}

void GEFont::release() {
    for (int i = 0; i < 95; ++i) {
        delete fontChar[i];
        fontChar[i] = nullptr;
    }
    GELog::shared().info("Font characters released.");
}