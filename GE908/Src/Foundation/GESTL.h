//
// Created by W.D. on 20/10/25
//

#pragma once

// GEVector
template<typename T>
class GEVector {

private:
    T* _data;
    size_t _size;
    size_t _capacity;

    void expand() {
        size_t newCapacity = (_capacity == 0) ? 1 : _capacity * 2;
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < _size; ++i) {
            newData[i] = _data[i];
        }
        delete[] _data;
        _data = newData;
        _capacity = newCapacity;
    }

public:
    GEVector() {}
    ~GEVector() { delete[] _data; }

    void append(const T& value) {
        if (_size >= _capacity) {
            expand();
        }
        _data[_size++] = value;
    }

    T& operator[](size_t index) {
        return _data[index];
    }

    const T& operator[](size_t index) const {
        return _data[index];
    }

    size_t size() const { return _size; }
    size_t capacity() const { return _capacity; }
};
