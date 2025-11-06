#include "GEFont.h"

Image* GEFont::fontChar[95] = { nullptr };
bool GEFont::isLoaded = false;

bool GEFont::load() {
    for (int i = 0;i < 95;++i) {
        std::string filename = "Src/Assets/Fonts/" + std::to_string(i + 32) + ".png";

        Image* img = new Image();

        if (!img->load(filename)) {
            GELog::shared().error("Failed to load font character: " + filename);
            delete img;
            for (int j = 0;j < i;++j) {
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
    for (int i = 0;i < 95;++i) {
        delete fontChar[i];
        fontChar[i] = nullptr;
    }
    GELog::shared().info("Font characters released.");
}

void GEFont::draw(const std::string& text, int startX, int startY, const unsigned char* textColor, int scale, GEWindow& window) {
    int cursorX = startX;
    int cursorY = startY;

    for (char c : text) {
        Image* img = GEFont::getChar(c);
        if (!img) continue;

        unsigned char* data = img->data;
        int originW = img->width;
        int originH = img->height;

        int scaledW = originW * scale;
        int scaledH = originH * scale;

        // loop through each pixel in the scaled image
        for (int sy = 0;sy < scaledH;sy++) {
            for (int sx = 0;sx < scaledW;sx++) {
                int srcX = sx / scale;
                int srcY = sy / scale;

                // after scale,the next char need to draw at cursorX + scaledW
                int idx = (srcY * originW + srcX) * 4;

                unsigned char originA = data[idx + 3];

                // tiny non-transparent pixel
                if (originA != 0) {
                    unsigned char r = textColor[0];
                    unsigned char g = textColor[1];
                    unsigned char b = textColor[2];
                    window.draw(cursorX + sx, cursorY + sy, r, g, b);
                }
            }
        }

        // update cursor position for next character
        cursorX += scaledW;
    }
}