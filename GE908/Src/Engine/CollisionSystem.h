#pragma once
#include "../Foundation/GEArray.h"
#include "../Foundation/GEObservable.h"
#include "../Foundation/GEEvent.h"
#include "../Core/Interfaces/ICollidable.h"

class CollisionSystem {
private:
    GEArray<ICollidable*> _colliders;

    void handleCollision(ICollidable* a, ICollidable* b) {
        a->onCollide(b);
        b->onCollide(a);
    }

public:
    CollisionSystem() = default;

    void registerCollider(ICollidable* c) {
        if (!c) return;

        _colliders.appendToFirstEmpty(c);
    }

    // unregister, make slot null
    void unregisterCollider(ICollidable* c) {
        if (!c) return;
        _colliders.clearValue(c);
    }

    void clear() { _colliders.clear(); }

    void update() {
        const unsigned int n = _colliders.size();
        for (unsigned int i = 0; i < n; ++i) {
            ICollidable* a = _colliders[i];
            if (!a) continue;

            for (unsigned int j = i + 1; j < n; ++j) {
                ICollidable* b = _colliders[j];
                if (!b) continue;

                if (a->collidesWith(*b))
                    handleCollision(a, b);
            }
        }
    }
};