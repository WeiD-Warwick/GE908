#pragma once
#include "GEArray.h"
#include "GEEvent.h"

class GEObserver {
public:
    virtual ~GEObserver() = default;
    virtual void onEvent(const GEEvent& event) = 0;
};

class GEObservable {
private:
    GEArray<GEObserver*> _observers;

    int findObserverIndex(GEObserver* observer) const {
        for (unsigned int i = 0; i < _observers.size(); ++i) {
            if (_observers[i] == observer) {
                return i;
            }
        }
        return -1;
    }

public:
    virtual ~GEObservable() = default;

    void addObserver(GEObserver* observer) {
        if (findObserverIndex(observer) == -1) {
            unsigned int newSize = _observers.size() + 1;
            _observers.resize(newSize);
            _observers[newSize - 1] = observer;
        }
    }

    void removeObserver(GEObserver* observer) {
        int index = findObserverIndex(observer);
        if (index == -1) return;

        unsigned int size = _observers.size();
        _observers[index] = _observers[size - 1];
        _observers.resize(size - 1);
    }

    void notify(const GEEvent& event) {
        for (unsigned int i = 0; i < _observers.size(); ++i) {
            _observers[i]->onEvent(event);
        }
    }
};

class GENotificationCenter : public GEObservable {

private:

    GENotificationCenter() = default;

public:

    static GENotificationCenter& shared() {
        static GENotificationCenter shared;
        return shared;
    }
};