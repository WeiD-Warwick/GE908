#include "GEWindow.h"
#include "../Core/Camera/GECamera.h"

using namespace GamesEngineeringBase;

GEWindow::GEWindow() {}

GEWindow::~GEWindow() {}

void GEWindow::load(int width, int height, const std::string& title, bool isFullstreen) {
    GELog::shared().info("Init window.");
    create(width, height, title, isFullstreen);
}

void GEWindow::drawText(const std::string& text, int startX, int startY, const unsigned char* textColor, int scale) {
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
        for (int sy = 0; sy < scaledH; sy++) {
            for (int sx = 0; sx < scaledW; sx++) {
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
                    draw(cursorX + sx, cursorY + sy, r, g, b);
                }
            }
        }

		// update cursor position for next character
        cursorX += scaledW;
    }
}

void GEWindow::drawMap(GEMapsManager& mapManager, const GECamera& camera) {
    const GESaveData* saveData = mapManager.getSaveData();
    if (!saveData) return;

    int layers = saveData->getLayerCount();
    int mapWidth = saveData->getMapColCount();
    int mapHeight = saveData->getMapRowCount();
    int tileWidth = saveData->getTileWidth();
    int tileHeight = saveData->getTileHeight();

    int winWidth = getWidth();
    int winHeight = getHeight();

    for (int layer = 0; layer < layers; layer++) {
        for (int row = 0; row < mapHeight; row++) {
            for (int col = 0; col < mapWidth; col++) {
                int tileID = saveData->getTileID(layer, row, col);
                Image* img = mapManager.getTileImage(tileID);
                if (!img) continue;

                int originX = col * tileWidth - camera.getOffsetX();
                int originY = row * tileHeight - camera.getOffsetY();

				// if the tile is completely outside the window, skip drawing
                if (originX + tileWidth < 0 || originX >= winWidth ||
                    originY + tileHeight < 0 || originY >= winHeight)
                    continue;

                for (unsigned int dy = 0; dy < img->height; dy++) {
                    for (unsigned int dx = 0; dx < img->width; dx++) {
                        int screenX = originX + dx;
                        int screenY = originY + dy;

						// border check
                        if (screenX < 0 || screenY < 0 ||
                            screenX >= winWidth || screenY >= winHeight)
                            continue;

                        if (img->alphaAt(dx, dy) > 210) {
                            unsigned char* px = img->at(dx, dy);
                            if (!px) continue;
                            draw(screenX, screenY, px);
                        }
                    }
                }
            }
        }
    }
}

void GEWindow::drawPlayer(const GEPlayer& player, const GECamera& camera) {
    const Image* characterImage = player.getCharacterImage();
    if (!characterImage) {
        GELog::shared().error("Player image is null, cannot render");
        return;
    }

    const unsigned char* pixelData = characterImage->data;
    int imageWidth = characterImage->width;
    int imageHeight = characterImage->height;
    int playerX = player.getX();
    int playerY = player.getY();

    for (int y = 0; y < imageHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {
            int pixelIndex = (y * imageWidth + x) * 4;
            unsigned char a = pixelData[pixelIndex + 3];

            if (a > 0) {
                int worldX = playerX + x;
                int worldY = playerY + y;
                int screenX = camera.worldToScreenX(worldX);
                int screenY = camera.worldToScreenY(worldY);
                draw(screenX, screenY,
                    pixelData[pixelIndex],
                    pixelData[pixelIndex + 1],
                    pixelData[pixelIndex + 2]);
            }
        }
    }
}
