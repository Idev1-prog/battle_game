#pragma once
#include <initializer_list>
#include <algorithm>

#define MEM_STEP 15

inline size_t calculate_capacity(size_t size) {
    if (size == 0) return MEM_STEP;
    return size + MEM_STEP;
}

template<typename T>
class TVector;

template<typename T>
class MemData {
protected:
    T* _data = nullptr;
    size_t _size;
    size_t _capacity;

public:
    MemData(size_t size = 0);
    MemData(std::initializer_list<T>);
    MemData(T*, size_t);
    MemData(const MemData& other);
    MemData(MemData&&) noexcept;
    ~MemData();

    inline bool is_empty() const noexcept { return _size == 0; }
    inline bool is_full() const noexcept { return _size == _capacity; }
    inline size_t size() const noexcept { return _size; }
    inline size_t capacity() const noexcept { return _capacity; }
    inline const T* data() const noexcept { return _data; }

    void set_memory(size_t) noexcept;
    void reset_memory(size_t size, size_t start_index = 0) noexcept;
    void clear_memory() noexcept;

    MemData& operator=(const MemData&) noexcept;
    MemData& operator=(MemData&&) noexcept;

    friend class TVector<T>;
};


template<typename T>
void MemData<T>::clear_memory() noexcept {
    if (_data != nullptr) {
        delete[] _data;
        _data = nullptr;
        _size = 0;
        _capacity = 0;
    }
}

template<typename T>
void MemData<T>::set_memory(size_t size) noexcept {
    clear_memory();
    _data = new T[size];
    _capacity = size;
}

template<typename T>
MemData<T>::MemData(size_t size) {
    _size = size;
    _capacity = calculate_capacity(_size);
    set_memory(_capacity);
    for (size_t i = 0; i < _size; ++i) _data[i] = T();
}

template<typename T>
MemData<T>::MemData(std::initializer_list<T> ini) {
    _size = ini.size();
    _capacity = calculate_capacity(_size);
    set_memory(_capacity);
    std::copy(ini.begin(), ini.end(), _data);
}

template<typename T>
MemData<T>::MemData(T* mass, size_t size) {
    _size = size;
    _capacity = calculate_capacity(_size);
    set_memory(_capacity);
    std::copy(mass, mass + size, _data);
}

template<typename T>
MemData<T>::MemData(const MemData& other) : _size(other._size), _capacity(other._capacity) {
    set_memory(_capacity);
    for (size_t i = 0; i < _size; ++i) {
        _data[i] = other._data[i];
    }
}

template<typename T>
MemData<T>::MemData(MemData&& other) noexcept
    : _size(other._size), _capacity(other._capacity), _data(other._data) {
    other._data = nullptr;
    other._size = 0;
    other._capacity = 0;
}

template<typename T>
MemData<T>::~MemData() {
    clear_memory();
}

template<typename T>
void MemData<T>::reset_memory(size_t size, size_t start_index) noexcept {
    if (size == _capacity) return;
    T* new_data = new T[size]();
    size_t copy_size = (_size < size) ? _size : size;
    for (size_t i = 0; i < copy_size; ++i) {
        new_data[i] = _data[(start_index + i) % _capacity];
    }
    delete[] _data;
    _data = new_data;
    _capacity = size;
    _size = copy_size;
}

template<typename T>
MemData<T>& MemData<T>::operator=(const MemData& other) noexcept {
    if (this == &other) return *this;
    set_memory(other._capacity);
    _size = other._size;
    _capacity = other._capacity;
    std::copy(other._data, other._data + _size, _data);
    return *this;
}

template<typename T>
MemData<T>& MemData<T>::operator=(MemData&& other) noexcept {
    if (this == &other) return *this;
    clear_memory();
    _data = other._data;
    _capacity = other._capacity;
    _size = other._size;
    other._data = nullptr;
    other._capacity = 0;
    other._size = 0;
    return *this;
}