#include "GEPlayer.h"

#define MOVE_SPEED 200.f

GEPlayer::GEPlayer() {}

void GEPlayer::load(const std::string& characterImagePath) {
	_characterImage = new Image();

	if (!_characterImage->load(characterImagePath)) {
		delete _characterImage;
		_characterImage = nullptr;
		GELog::shared().error("Load player character image: " + characterImagePath + "Failed");
	} else {
		_width = _characterImage->width;
		_height = _characterImage->height;
		GELog::shared().info("Load player character image: " + characterImagePath + "Success");
	}
}

void GEPlayer::update(float deltaTime, bool moveUp, bool moveDown, bool moveLeft, bool moveRight) {
    float moveDelta = _speed * deltaTime;
    unsigned int moveAmount = static_cast<unsigned int>(max(moveDelta, 1.0f)); 

    if (moveUp) _y -= moveAmount;
    if (moveDown) _y += moveAmount;
    if (moveLeft) _x -= moveAmount;
    if (moveRight) _x += moveAmount;

    std::cout << _x << _y << std::endl;
}

void GEPlayer::render(GEWindow& window, int cameraOffsetX, int cameraOffsetY) const {
    if (!_characterImage) {
        GELog::shared().warning("Player image is null, cannot render");
        return;
    }

    int screenStartX = _x - cameraOffsetX;
    int screenStartY = _y - cameraOffsetY;

    unsigned char* pixelData = _characterImage->data;
    int imageWidth = _characterImage->width;
    int imageHeight = _characterImage->height;

    for (int y = 0; y < imageHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {
            int pixelIndex = (y * imageWidth + x) * 4;

            unsigned char r = pixelData[pixelIndex];     
            unsigned char g = pixelData[pixelIndex + 1]; 
            unsigned char b = pixelData[pixelIndex + 2]; 
            unsigned char a = pixelData[pixelIndex + 3];


            if (a > 0) {
                int screenX = screenStartX + x;
                int screenY = screenStartY + y;
                window.draw(screenX, screenY, r, g, b);
            }
        }
    }
}
