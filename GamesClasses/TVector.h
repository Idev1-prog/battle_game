#pragma once
#include <iostream>
#include "memdata.h"

template<typename T>
class TVector {
    MemData<T> _mem;
    size_t _front;
    size_t _back;

    void optimize_mem() noexcept;
    void advance_front() noexcept;
    void retreat_front() noexcept;
    void advance_back() noexcept;
    void retreat_back() noexcept;
    size_t get_index(size_t) const noexcept;

public:
    TVector(size_t size = 0);
    TVector(std::initializer_list<T> ini);
    TVector(T* data, size_t size);
    TVector(const TVector& other);
    TVector(TVector&&);
    ~TVector() = default;

    inline void manual_memory_initialization(int size) noexcept {
        _mem.set_memory(size); 
        _front = 0;
        _mem._size = size;
        _back = (_mem._size > 0) ? _mem._size - 1 : 0;
    }
    inline bool is_empty() const noexcept { return _mem.is_empty(); }
    inline bool is_full() const noexcept { return _mem.is_full(); }
    inline size_t size() const noexcept { return _mem.size(); }
    inline size_t capacity() const noexcept { return _mem.capacity(); }

    inline T& front() {
        if (!_mem.size()) throw std::out_of_range("Empty vector");
        return _mem._data[_front];
    }
    inline const T& front() const {
        if (!_mem.size()) throw std::out_of_range("Empty vector");
        return _mem._data[_front];
    }
    inline T& back() {
        if (!_mem.size()) throw std::out_of_range("Empty vector");
        return _mem._data[_back];
    }
    inline const T& back() const {
        if (!_mem.size()) throw std::out_of_range("Empty vector");
        return _mem._data[_back];
    }

    void push_front(const T&) noexcept;
    void push_back(const T&) noexcept;
    void insert(const T&, size_t);
    void pop_front();
    void pop_back();
    void erase(size_t);

    void pop_front_few(size_t count) { for (size_t i = 0; i < count; ++i) pop_front(); }
    void pop_back_few(size_t count) { for (size_t i = 0; i < count; ++i) pop_back(); }
    void push_front_few(const T*, size_t) noexcept;
    void push_back_few(const T*, size_t) noexcept;

    TVector& operator=(const TVector&) noexcept;
    TVector& operator=(TVector&&) noexcept;
    T& operator[](size_t) noexcept;
    const T& operator[](size_t) const noexcept;

    template<typename T>
    friend std::ostream& operator<<(std::ostream&, const TVector<T>&);
    template<typename T>
    friend std::istream& operator>>(std::istream&, TVector<T>&);
};


template<typename T>
void TVector<T>::optimize_mem() noexcept {
    bool reallocated = false;
    size_t new_capacity;
    if (_mem._size == _mem._capacity) {
        new_capacity = calculate_capacity(_mem._size);
        reallocated = true;
    }
    else if ((_mem._capacity - _mem._size) > MEM_STEP && _mem._capacity > MEM_STEP) {
        new_capacity = _mem._capacity - MEM_STEP;
        reallocated = true;
    }
    if (reallocated) {
        _mem.reset_memory(new_capacity, _front);
        _front = 0;
        _back = (_mem._size > 0) ? _mem._size - 1 : 0;
    }
    if (_mem._size == 0) {
        _front = 0;
        _back = 0;
    }
}

template<typename T>
void TVector<T>::advance_front() noexcept {
    _front = (_front + 1) % _mem.capacity();
}
template<typename T>
void TVector<T>::retreat_front() noexcept {
    _front = (_front == 0) ? _mem.capacity() - 1 : _front - 1;
}
template<typename T>
void TVector<T>::advance_back() noexcept {
    _back = (_back + 1) % _mem.capacity();
}
template<typename T>
void TVector<T>::retreat_back() noexcept {
    _back = (_back == 0) ? _mem.capacity() - 1 : _back - 1;
}

template<typename T>
size_t TVector<T>::get_index(size_t logical_index) const noexcept {
    return (logical_index + _front) % _mem.capacity();
}

template<typename T>
TVector<T>::TVector(size_t size) : _mem(size), _front(0), _back((size > 0) ? size - 1 : 0) {}

template<typename T>
TVector<T>::TVector(std::initializer_list<T> ini) : _mem(ini), _front(0), _back((ini.size() > 0) ? ini.size() - 1 : 0) {}

template<typename T>
TVector<T>::TVector(T* data, size_t size) : _mem(data, size), _front(0), _back((size > 0) ? size - 1 : 0) {}

template<typename T>
TVector<T>::TVector(const TVector& other) : _mem(other._mem), _front(other._front), _back(other._back) {}

template<typename T>
TVector<T>::TVector(TVector&& other)
    : _mem(std::move(other._mem)), _front(other._front), _back(other._back) {
    other._front = 0;
    other._back = 0;
}

template<typename T>
void TVector<T>::push_front(const T& num) noexcept {
    optimize_mem();
    retreat_front();
    _mem._data[_front] = num;
    _mem._size++;
}

template<typename T>
void TVector<T>::push_back(const T& num) noexcept {
    optimize_mem();
    if (_mem._size > 0) advance_back();
    _mem._data[_back] = num;
    _mem._size++;
}

template<typename T>
void TVector<T>::pop_front() {
    if (_mem._size <= 0) throw std::logic_error("You can't delete something that doesn't exist.");
    _mem._data[_front] = T();
    advance_front();
    _mem._size--;
    optimize_mem();
}

template<typename T>
void TVector<T>::pop_back() {
    if (_mem._size <= 0) throw std::logic_error("You can't delete something that doesn't exist.");
    _mem._data[_back] = T();
    retreat_back();
    _mem._size--;
    optimize_mem();
}

template<typename T>
void TVector<T>::insert(const T& num, size_t index) {
    if (index > _mem._size) throw std::out_of_range("Insert index out of range");
    optimize_mem();
    if (_mem._size == 0) { push_back(num); return; }
    if (index == 0) { push_front(num); return; }
    if (index == _mem._size) { push_back(num); return; }

    advance_back();
    _mem._size++;
    for (size_t i = _mem._size - 1; i > index; --i) {
        (*this)[i] = (*this)[i - 1];
    }
    (*this)[index] = num;
}

template<typename T>
void TVector<T>::erase(size_t index) {
    if (index >= _mem._size) throw std::out_of_range("Erase index out of range");
    if (_mem._size == 0) throw std::logic_error("Cannot erase from empty vector");
    optimize_mem();
    if (_mem._size == 1) { pop_back(); return; }
    if (index == 0) { pop_front(); return; }
    if (index == (_mem._size - 1)) { pop_back(); return; }

    for (size_t i = index; i < _mem._size - 1; ++i) {
        (*this)[i] = (*this)[i + 1];
    }
    _mem._size--;
    retreat_back();
}

template<typename T>
void TVector<T>::push_front_few(const T* numbers, size_t count) noexcept {
    for (size_t i = 0; i < count; ++i) push_front(numbers[i]);
}

template<typename T>
void TVector<T>::push_back_few(const T* numbers, size_t count) noexcept {
    for (size_t i = 0; i < count; ++i) push_back(numbers[i]);
}

template<typename T>
T& TVector<T>::operator[](size_t index) noexcept {
    return _mem._data[(_front + index) % _mem._capacity];
}

template<typename T>
const T& TVector<T>::operator[](size_t index) const noexcept {
    return _mem._data[(_front + index) % _mem._capacity];
}

template<typename T>
TVector<T>& TVector<T>::operator=(const TVector& other) noexcept {
    if (this != &other) {
        _mem = other._mem;
        _front = other._front;
        _back = other._back;
    }
    return *this;
}

template<typename T>
TVector<T>& TVector<T>::operator=(TVector&& other) noexcept {
    if (this != &other) {
        _mem = std::move(other._mem);
        _front = other._front;
        _back = other._back;
        other._front = 0;
        other._back = 0;
    }
    return *this;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const TVector<T>& vec) {
    out << "{ ";
    size_t sz = vec._mem.size();
    for (size_t i = 0; i < sz; ++i) {
        out << vec[i];
        if (i < sz - 1) out << ", ";
    }
    out << " }";
    return out;
}

template<typename T>
std::istream& operator>>(std::istream& in, TVector<T>& vec) {
    size_t expected_size;
    if (!(in >> expected_size)) return in;
    if (vec.size() > 0) vec._mem.clear_memory();
    T value;
    for (size_t i = 0; i < expected_size && (in >> value); ++i) {
        vec.push_back(value);
    }
    return in;
}




//Возможное решения проблемы с многомерными массивами (ИИ-решение):
//template<typename T>
//void TVector<T>::resize(size_t new_size) {
//    // 1. Если новый размер больше текущего, нужно расширить память
//    if (new_size > _mem.capacity()) {
//        // Используем ваш существующий механизм перевыделения
//        // Но будьте осторожны: reset_memory может менять capacity и копировать данные
//        _mem.reset_memory(calculate_capacity(new_size), _front);
//        // После reset_memory _front обычно сбрасывается в 0, а _back корректируется
//        _front = 0;
//    }
//
//    // 2. Самое главное: обновляем логический размер
//    // Если мы уменьшаем вектор, просто меняем size.
//    // Если увеличиваем, новые элементы должны быть инициализированы T()
//
//    size_t old_size = _mem.size(); // Внимание: убедитесь, что _mem хранит актуальный size
//
//    if (new_size > old_size) {
//        // Инициализируем новые ячейки значениями по умолчанию
//        for (size_t i = old_size; i < new_size; ++i) {
//            // Важно: обращаться нужно через правильный индекс, если вектор циклический
//            // Но если мы только что сделали reset_memory и front=0, то можно прямо:
//            _mem._data[i] = T();
//        }
//    }
//
//    // 3. Обновляем внутренние счетчики TVector
//    _mem._size = new_size; // Или как у вас называется поле размера в MemData
//
//    if (new_size == 0) {
//        _front = 0;
//        _back = 0; // Или -1, зависит от вашей логики empty vector
//    }
//    else {
//        // Если мы не трогали front, то back сдвигается
//        // В простейшем случае (linear storage after resize):
//        _front = 0;
//        _back = new_size - 1;
//    }
//}