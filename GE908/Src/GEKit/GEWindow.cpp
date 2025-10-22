//
// Created by W.D. on 20/10/25
//

#include "GEWindow.h"
#include "../Foundation/GELog.h"
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Assets/GEResource.h"

using namespace GamesEngineeringBase;

GEWindow::GEWindow() {}

GEWindow::~GEWindow() {}

// launch App Animation
// Gradient effect. Black -> White
void GEWindow::playLaunchAppAnimation(float duration) {
	loadFont();
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

// draw the backgroundColor
void GEWindow::drawBGColor() {
    clear();

    for (unsigned int i = 0; i < (getWidth() * getHeight()); i++) {
        draw(i, backgroundColor.r, backgroundColor.g, backgroundColor.b);
    }
}

void GEWindow::loadFont() {
	Image fontImage;
	fontImage.load("..\\..\\Assets\\Fonts\\nullptr.png");
}

void GEWindow::drawText(const std::string& text, int startX, int startY, const GEColor& color, int scale = 1) {

}


void GEWindow::render() {
    drawBGColor();
    present();
}