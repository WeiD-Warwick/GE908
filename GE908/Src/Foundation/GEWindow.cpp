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
