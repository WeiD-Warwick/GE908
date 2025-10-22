#include "GEModel.h"

// Color presets
const GEColor GEColor::BackgroundDark(30, 30, 40);
const GEColor GEColor::BackgroundLight(220, 220, 220);
const GEColor GEColor::White(255, 255, 255);
const GEColor GEColor::Red(220, 20, 60);
const GEColor GEColor::Green(34, 139, 34);
const GEColor GEColor::Blue(30, 144, 255);
const GEColor GEColor::Yellow(255, 215, 0);
const GEColor GEColor::Orange(255, 140, 0);

GEColor::GEColor(unsigned char r, unsigned char g, unsigned char b)
	: r(r), g(g), b(b) {
}