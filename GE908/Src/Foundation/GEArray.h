#pragma once

template <typename T>
class GEArray {
private:
    T* _data = nullptr;
    unsigned int _size = 0;

    void swap(T& a, T& b) {
        T temp = a;
        a = b;
        b = temp;
    }

    unsigned int max_index_in_range(unsigned int range) const {
        unsigned int max_idx = 0;
        for (unsigned int i = 1; i < range; ++i)
            if (_data[i] >= _data[max_idx])
                max_idx = i;
        return max_idx;
    }

public:
    GEArray() = default;

    explicit GEArray(unsigned int count) {
        resize(count);
    }

    ~GEArray() {
        delete[] _data;
    }

    GEArray(const GEArray&) = delete;
    GEArray& operator=(const GEArray&) = delete;

    unsigned int size() const { return _size; }

    bool empty() const { return _size == 0; }

    T& operator[](unsigned int i) { return _data[i]; }
    const T& operator[](unsigned int i) const { return _data[i]; }

    T* data() { return _data; }
    const T* data() const { return _data; }

    void resize(unsigned int new_size) {
        if (new_size == _size) return;

        T* new_data = nullptr;
        if (new_size > 0) {
            new_data = new T[new_size];
            unsigned int copy_count = (new_size < _size) ? new_size : _size;
            for (unsigned int i = 0; i < copy_count; ++i) {
                new_data[i] = _data[i];
            }
        }
    }

    void clear() {
        delete[] _data;
        _data = nullptr;
        _size = 0;
    }

    void fill(const T& value) {
        for (unsigned int i = 0; i < _size; ++i)
            _data[i] = value;
    }

    T max_element() const {
        if (_size == 0) return T{};
        T max_val = _data[0];
        for (unsigned int i = 1; i < _size; ++i)
            if (_data[i] > max_val)
                max_val = _data[i];
        return max_val;
    }

    unsigned int max_index() const {
        if (_size == 0) return 0;
        unsigned int max_idx = 0;
        for (unsigned int i = 1; i < _size; ++i)
            if (_data[i] >= _data[max_idx])
                max_idx = i;
        return max_idx;
    }

    void sort() {
        for (unsigned int i = _size; i > 0; --i) {
            unsigned int max_idx = max_index_in_range(i);
            swap(_data[max_idx], _data[i - 1]);
        }
    }
};
