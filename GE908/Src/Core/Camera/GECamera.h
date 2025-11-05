#pragma once

class GECamera {
private:
	float _x = 0;
	float _y = 0;

	int _width = 0;
	int _height = 0;

	int _mapWidth = 0;
	int _mapHeight = 0;

public:

	GECamera() {}
	~GECamera() {}

    void load(int windowWidth, int windowHeight, int mapWidth, int mapHeight);

    void followPlayer(float playerX, float playerY, int playerWidth, int playerHeight);

	float getX() const { return _x; }
	float getY() const { return _y; }

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

	void setMapBounds(int mapWidth, int mapHeight);
};