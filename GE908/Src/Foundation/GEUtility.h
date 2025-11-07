#pragma once
#include <random>

static float clamp(float value, float minVal, float maxVal) {
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;
	return value;
}


static bool circleRectCollision(float circleX, float circleY, float radius,
    float rectLeft, float rectTop, float rectRight, float rectBottom) {
    float closestX = clamp(circleX, rectLeft, rectRight);
    float closestY = clamp(circleY, rectTop, rectBottom);

    float dx = circleX - closestX;
    float dy = circleY - closestY;

    return (dx * dx + dy * dy) <= (radius * radius);
}

static float randomFloat(float from = 0.0f, float to = 1.0f) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(from, to);
    return dist(gen);
}


struct GEColor {
    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;

    GEColor() = default;
    GEColor(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
};

struct GEPoint {
    float x = 0.0f;
    float y = 0.0f;

    GEPoint(float x_, float y_) : x(x_), y(y_) {}
    GEPoint(int x_, int y_) : x(static_cast<float>(x_)), y(static_cast<float>(y_)) {}
};

struct GESize {
    float width = 0.0f;
    float height = 0.0f;

    GESize(float width_, float height_) : width(width_), height(height_) {}
    GESize(int width_, int height_) : width(static_cast<float>(width_)), height(static_cast<float>(height_)) {}
};