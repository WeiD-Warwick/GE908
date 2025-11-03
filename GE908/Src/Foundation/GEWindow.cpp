#include "GEWindow.h"
#include "../Core/Camera/GECamera.h"

using namespace GamesEngineeringBase;

GEWindow::GEWindow() {}

GEWindow::~GEWindow() {}

void GEWindow::load(int width, int height, const std::string& title, bool isFullstreen) {
    GELog::shared().info("Init window.");
    create(width, height, title, isFullstreen);
}
