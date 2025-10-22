#include "GEWindow.h"

using namespace GamesEngineeringBase;

void GEWindow::drawBGColor() {
    clear();
    for (unsigned int i = 0; i < (getWidth() * getHeight()); i++) {
        draw(i, backgroundColor.r, backgroundColor.g, backgroundColor.b);
    }
}

GEWindow::GEWindow() {}

GEWindow::~GEWindow() {}

void GEWindow::loadWindow() {
    GELog::shared().info("Init window.");
    create(1280, 720, "WM908", false);
    _playLaunchAppAnimation(5.f);
}

void GEWindow::_playLaunchAppAnimation(float duration) {
    GELog::shared().info("Begin Play Launch App Animation");

    const GEColor startColor(0, 0, 0);
    const GEColor endColor(255, 255, 255);
    const int steps = 100;
    const float stepTime = duration / steps;

    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;

        int r = startColor.r + t * (endColor.r - startColor.r);
        int g = startColor.g + t * (endColor.g - startColor.g);
        int b = startColor.b + t * (endColor.b - startColor.b);

        backgroundColor = GEColor(r, g, b);
        drawBGColor();
        present();
    }

    GELog::shared().info("End Play Launch App Animation");
}

void GEWindow::drawText(const std::string& text, int startX, int startY, const GEColor& color, int scale) {
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
        for (int sy = 0; sy < scaledH; ++sy) {
            for (int sx = 0; sx < scaledW; ++sx) {
                int srcX = sx / scale; 
                int srcY = sy / scale;

				// after scale,the next char need to draw at cursorX + scaledW
                int idx = (srcY * originW + srcX) * 4;

                unsigned char originA = data[idx + 3];

				// tiny non-transparent pixel
                if (originA != 0) {
                    unsigned char r = color.r;
                    unsigned char g = color.g;
                    unsigned char b = color.b;
                    draw(cursorX + sx, cursorY + sy, r, g, b);
                }
            }
        }

		// update cursor position for next character
        cursorX += scaledW;
    }
}

void GEWindow::render() {
    drawBGColor();
    drawText("Welcome To GE908!", 50, 50, GEColor::Red, 2);
    present();
}