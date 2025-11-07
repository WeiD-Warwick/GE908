#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GECollisible.h"

using namespace GamesEngineeringBase;

class BaseCharacter : public GECollisible {

protected:
	int _width = 0;
	int _height = 0;
	int _speed = 70;
	int _hp = 0;
    int _maxHp = 0;
	int _mapWidth = 0;
	int _mapHeight = 0;

    float _accumX = 0.0f;
    float _accumY = 0.0f;

    float _damageFlashTimer = 0.0f;
    float _damageFlashDuration = 0.0f;
    unsigned char _damageFlashR = 255;
    unsigned char _damageFlashG = 255;
    unsigned char _damageFlashB = 255;

    float _damageNumberTimer = 0.0f;
    float _damageNumberDuration = 0.0f;
    unsigned char _damageNumberR = 255;
    unsigned char _damageNumberG = 255;
    unsigned char _damageNumberB = 255;

    float _contactDamageCooldownTimer = 0.0f;
    float _contactDamageCooldownDuration = 0.5f;

    virtual bool isBlockedAt(float x, float y) const { return false; }

    void moveUpdate(float deltaTime, float dirX, float dirY);

    virtual void applyMovementBounds(float& newX, float& newY) = 0;

    virtual void applyEnvironmentalEffects(float deltaTime) {}
    void triggerDamageFlash(unsigned char r, unsigned char g, unsigned char b, float duration);
    void updateCharacterState(float deltaTime);
    void draw(Window& window, const GECamera& camera) override;
    void drawHP(Window& window, const GECamera& camera);
    void drawHurt(Window& window, const GECamera& camera);
    void setContactDamageCooldownDuration(float duration) { _contactDamageCooldownDuration = max(0.0f, duration); }

public:
    BaseCharacter(const std::string& filename = "", GECollisionType type = GECollisionType::None)
        : GECollisible(filename, type) {
        _width = _image.width;
        _height = _image.height;
    }

    virtual ~BaseCharacter() {}
    virtual void takeDamage(int value);

    bool canReceiveContactDamage() const { return _contactDamageCooldownTimer <= 0.0f; }
    void startContactDamageCooldown();

    int getSpeed() const { return _speed; }
    int getHP() const { return _hp; }
    bool isAlive() const { return _hp > 0; }
    void setSpeed(int speed) { _speed = speed; }
	void setMapBounds(int mapWidth, int mapHeight) { _mapWidth = mapWidth;_mapHeight = mapHeight;}
};
