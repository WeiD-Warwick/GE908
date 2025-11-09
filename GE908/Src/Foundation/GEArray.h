#pragma once

template <typename T>
class GEArray {
private:
    T* _data = nullptr;
    unsigned int _size = 0;
    unsigned int _capacity = 0;

public:
    GEArray() = default;

    explicit GEArray(unsigned int capacity) { reserve(capacity); }

    ~GEArray() { delete[] _data; }

    GEArray(const GEArray&) = delete;
    GEArray& operator=(const GEArray&) = delete;

    unsigned int size() const { return _size; }

    bool empty() const { return _size == 0; }

    T& operator[](unsigned int i) { return _data[i]; }
    const T& operator[](unsigned int i) const { return _data[i]; }

    T* data() { return _data; }
    const T* data() const { return _data; }

    void reserve(unsigned int new_capacity) {
        if (new_capacity <= _capacity) return;

        T* new_data = new T[new_capacity];
        for (unsigned int i = 0; i < _size; ++i)
            new_data[i] = _data[i];

        delete[] _data;
        _data = new_data;
        _capacity = new_capacity;
    }

    void resize(unsigned int new_size) {
        if (new_size > _capacity)
            reserve(new_size);

        _size = new_size;
    }

    void append(const T& value) {
        if (_size >= _capacity)
            reserve((_capacity == 0) ? 4 : _capacity * 2);

        _data[_size++] = value;
    }

    void appendToFirstEmpty(const T& value) {
        for (unsigned int i = 0; i < _size; ++i) {
            if (_data[i] == nullptr) {
                _data[i] = value;
                return;
            }
        }
        if (_size >= _capacity)
            reserve((_capacity == 0) ? 4 : _capacity * 2);

        _data[_size++] = value;
    }

    void clearValue(const T& value) {
        for (unsigned int i = 0; i < _size; ++i) {
            if (_data[i] == value) {
                _data[i] = nullptr;
                return;
            }
        }
    }

    void clear() {
        delete[] _data;
        _data = nullptr;
        _size = 0;
        _capacity = 0;
    }

    void fill(const T& value) {
        for (unsigned int i = 0; i < _size; ++i)
            _data[i] = value;
    }
};
