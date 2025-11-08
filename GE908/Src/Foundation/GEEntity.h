#pragma once
#include "GEModel.h"
#include "GEUtility.h"

class GEEntity {
protected:
    float _centerX;
    float _centerY;
public:
    virtual ~GEEntity() = default;

    // Return the collider type of this entity
    virtual GEColliderType getColliderType() const = 0;

    // Return the collider of this entity
    virtual const GECollider& getCollider() const = 0;

    // Return if this entity is still active / alive
    virtual bool isAlive() const = 0;

    // Return the world position of the entity's left-top
    virtual GEPoint getOriginPoint() const = 0;

    // Return the world position of the entity's center
    virtual GEPoint getCenterPoint() const = 0;

    // Return the size of the entity
    virtual GESize getSize() const = 0;

    // Applies damage to the entity
    virtual void takeDamage(int damage) {}

    // Called when the entity is destroyed or removed from world
    virtual void onDestroy() {}
};