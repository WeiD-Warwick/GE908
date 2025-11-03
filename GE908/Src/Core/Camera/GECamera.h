#pragma once

class GECamera {
private:
	int _x = 0;
	int _y = 0;

	int _width = 0;
	int _height = 0;

	int _mapWidth = 0;
	int _mapHeight = 0;

public:

	GECamera();
	~GECamera();

    void load(int windowWidth, int windowHeight, int mapWidth, int mapHeight);

    void followPlayer(int playerX, int playerY, int playerWidth, int playerHeight);

    int getX() const { return _x; }
    int getY() const { return _y; }

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

	void setMapBounds(int mapWidth, int mapHeight);
};