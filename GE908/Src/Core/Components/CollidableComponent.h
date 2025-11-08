#pragma once
#include "../Interfaces/ICollidable.h"
#include "../../Foundation/GEUtility.h"
#include "../../Foundation/GEModel.h"

class GEContext;

typedef void(*CollisionCallback)(class ICollidable*, class ICollidable*);

class CollidableComponent : public ICollidable {

private:
    GEPoint _originPoint;
    GESize  _size;
    GEColliderType  _type;
    GEColliderShape _shape;
    CollisionCallback _onCollisionBlock = nullptr;
    GEContext* _ctx = nullptr;

public:
    CollidableComponent(GEPoint originPoint = GEPoint(0, 0),
        GESize size = GESize(0, 0),
        GEColliderType type = GEColliderType::None,
        GEColliderShape shape = GEColliderShape::AABB,
        GEContext* ctx = nullptr)
        : _originPoint(originPoint), _size(size),
        _type(type), _shape(shape), _ctx(ctx)
    {
        if (_ctx && _ctx->collision())
            _ctx->collision()->registerCollider(this);
    }

    ~CollidableComponent() {
        if (_ctx && _ctx->collision())
            _ctx->collision()->unregisterCollider(this);
    }

    // ========== Getters ==========
    GEColliderShape getShape() const override { return _shape; }
    GEColliderType  getType()  const override { return _type; }
    GESize          getSize()  const override { return _size; }

    GEPoint getOriginPosition() const override { return _originPoint; }

    GEPoint getCenterPosition() const override {
        return GEPoint(_originPoint.x + _size.width * 0.5f,
            _originPoint.y + _size.height * 0.5f);
    }

    // ========== Setters ==========
    void setOriginPosition(GEPoint originPoint) { _originPoint = originPoint; }
    void setSize(GESize size) { _size = size; }
    void setType(GEColliderType type) { _type = type; }
    void setShape(GEColliderShape shape) { _shape = shape; }
    void setOnCollision(CollisionCallback cb) { _onCollisionBlock = cb; }

    // ========== Hooks ==========
    void onCollide(ICollidable* other) override {
        if (!_onCollisionBlock) return;

        auto otherComp = dynamic_cast<CollidableComponent*>(other);
        _onCollisionBlock(this, otherComp);
    }
};
