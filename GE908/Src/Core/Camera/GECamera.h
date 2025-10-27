//
// Created by W.D. on 22/10/25
//

#pragma once

class GECamera {
private:
	int _offsetX;                // camera offset X   
	int _offsetY;                // camer offset Y
	int _screenW = 0;            // world coordinate x of the top-left corner of the camera view
	int _screenH = 0;            // world coordinate y of the top-left corner of the camera view

	int _mapWidth = 0;
	int _mapHeight = 0;

public:

    GECamera(int screenW, int screenH);
	~GECamera();

    void followPlayer(int playerX, int playerY, int playerWidth, int playerHeight);
	void setMapBounds(int mapWidth, int mapHeight);

    int worldToScreenX(int worldX) const { return worldX - _offsetX; }
    int worldToScreenY(int worldY) const { return worldY - _offsetY; }
	int screenToWorldX(int screenX) const { return screenX + _offsetX; }
	int screenToWorldY(int screenY) const { return screenY + _offsetY; }

	int getOffsetX() const { return _offsetX; }
	int getOffsetY() const { return _offsetY; }
};