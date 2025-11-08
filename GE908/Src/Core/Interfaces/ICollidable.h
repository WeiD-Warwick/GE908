#pragma once
#include "../../Foundation/GEUtility.h"
#include "../../Foundation/GEModel.h"

class ICollidable {
public:
    virtual GEColliderShape getShape() const = 0;
    virtual GEColliderType  getType() const = 0;
    virtual GESize          getSize() const = 0;
    virtual GEPoint         getOriginPosition() const = 0;
    virtual GEPoint         getCenterPosition() const = 0;

	virtual void onCollide(ICollidable* other) = 0;

    virtual bool collidesWith(const ICollidable& other) const {
        const GEColliderShape s1 = getShape();
        const GEColliderShape s2 = other.getShape();

        if (s1 == GEColliderShape::AABB && s2 == GEColliderShape::AABB)
            return rectRectCollision(*this, other);
        else if (s1 == GEColliderShape::Circle && s2 == GEColliderShape::Circle)
            return circleCircleCollision(*this, other);
        else
            return rectCircleCollision(*this, other);
    }

    virtual ~ICollidable() = default;

protected:
	// AABB <-> AABB
	static bool rectRectCollision(const ICollidable& a, const ICollidable& b) {
		const GEPoint& aPos = a.getOriginPosition();
		const GESize& aSize = a.getSize();

		const GEPoint& bPos = b.getOriginPosition();
		const GESize& bSize = b.getSize();

		float aLeft = aPos.x;
		float aRight = aPos.x + aSize.width;
		float aTop = aPos.y;
		float aBottom = aPos.y + aSize.height;

		float bLeft = bPos.x;
		float bRight = bPos.x + bSize.width;
		float bTop = bPos.y;
		float bBottom = bPos.y + bSize.height;

		bool separatedHorizontally = (aRight < bLeft) || (aLeft > bRight);
		bool separatedVertically = (aBottom < bTop) || (aTop > bBottom);

		return !(separatedHorizontally || separatedVertically);
	}

	// AABB <-> Circle
	static bool circleCircleCollision(const ICollidable& a, const ICollidable& b) {
		GEPoint centerA = a.getCenterPosition();
		GEPoint centerB = b.getCenterPosition();

		float dx = centerA.x - centerB.x;
		float dy = centerA.y - centerB.y;
		float distanceSquared = dx * dx + dy * dy;

		float radiusA = a.getSize().width * 0.5f;
		float radiusB = b.getSize().width * 0.5f;
		float radiusSum = radiusA + radiusB;

		return distanceSquared <= (radiusSum * radiusSum);
	}


	// AABB <-> Circle
	static bool rectCircleCollision(const ICollidable& rect, const ICollidable& circle) {
		GEPoint circleCenter = circle.getCenterPosition();
		float   radius = circle.getSize().width * 0.5f;

		float rectLeft = rect.getOriginPosition().x;
		float rectTop = rect.getOriginPosition().y;
		float rectRight = rectLeft + rect.getSize().width;
		float rectBottom = rectTop + rect.getSize().height;


		float closestX = clamp(circleCenter.x, rectLeft, rectRight);
		float closestY = clamp(circleCenter.y, rectTop, rectBottom);

		float dx = circleCenter.x - closestX;
		float dy = circleCenter.y - closestY;

		return (dx * dx + dy * dy) <= (radius * radius);
	}
};
