#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>

namespace Linear {
    template <typename T>
    class Vector;
    template <typename T>
    class VectorIterator;

    template <typename T>
    class VectorIterator {
    private:
        T* current_;
    
    public:
        explicit VectorIterator(T* data_) : current_(data_) {}

        T& operator*() {
            return *current_;
        }

        VectorIterator& operator++() {
            current_ = ++current_;
            return *this;
        }

        VectorIterator& operator--() {
            current_ = --current_;
            return *this;
        }

        bool operator==(const VectorIterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const VectorIterator& other) const {
            return !(*this == other);
        }

        friend class Vector<T>;
    };

    template <typename T>
    class Vector {
    private:
        T* data_;
        size_t size_;
        size_t capacity_;
    
    public:
        explicit Vector() : data_(nullptr), size_(0), capacity_(0) {}
        explicit Vector(size_t initial_capacity)
            : data_(static_cast<T*>(::operator new(initial_capacity * sizeof(T)))),
              size_(0),
              capacity_(initial_capacity) {}
        explicit Vector(size_t count, const T& val) {
            data_ = static_cast<T*>(::operator new(count * sizeof(T)));
            capacity_ = count;

            for (size_t i = 0; i < count; i++) {
                new (data_+i) T(val);
            }

            size_ = count;
        }
        explicit Vector(const Vector& other) {
            data_ = static_cast<T*>(::operator new(other.capacity_ * sizeof(T)));
            capacity_ = other.capacity_;

            for (int i = 0; i < other.size_; i++) {
                new (data_+i) T(other.data_[i]);
            }
            size_ = other.size_;
        }
        
        ~Vector() {
            clear();
            ::operator delete(data_);
        }

        Vector& operator=(const Vector& other) {
            size_t temp_size = other.size_;
            clear();
            ::operator delete(data_);

            data_ = static_cast<T*>(::operator new(other.capacity_ * sizeof(T)));
            capacity_ = other.capacity_;

            for (int i = 0; i < other.size_; i++) {
                new (data_+i) T(other.data_[i]);
            }
            size_ = temp_size;
            return *this;
        }

        T& operator[](size_t index) {
            return data_[index];
        }

        const T& operator[](size_t index) const {
            return data_[index];
        }

        T& at(size_t index) {
            if (index >= size_) throw std::out_of_range("Index is out");
            return data_[index];
        }

        void reserve(size_t new_capacity) {
            if (new_capacity <= capacity_) return;
            T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
            for (size_t i = 0; i < size_; i++) {
                new (new_data + i) T(std::move(data_[i]));
                data_[i].~T();
            }
            ::operator delete(data_);
            data_ = new_data;
            capacity_ = new_capacity;
        }

        void push_back(const T& val) {
            if (size_ >= capacity_) {
                reserve(capacity_ == 0 ? 1 : capacity_ * 2);
            }
            new (data_ + size_) T(val);
            ++size_;
        }

        void push_back(T&& val) {
            if (size_ >= capacity_) {
                reserve(capacity_ == 0 ? 1 : capacity_ * 2);
            }
            new (data_ + size_) T(std::move(val));
            ++size_;
        }

        size_t size() {
            return size_;
        }

        size_t capacity() {
            return capacity_;
        }

        bool empty() {
            return size_ == 0;
        }

        void clear() {
            for (size_t i = 0; i < size_; i++) {
                data_[i].~T();
            }
            size_ = 0;
        }

        void erase (size_t index) {
            for (size_t i = index + 1; i < size_; i++) {
                data_[i-1] = data_[i];
            }
            size_ -= 1;
        }

        void insert (size_t index, const T& val) {
            if (size_ >= capacity_) {
                reserve(capacity_ == 0 ? 1 : capacity_ * 2);
            }

            for (size_t i = size_; i > index; i--) {
                if (i < capacity_) { 
                    data_[i] = data_[i-1];
                }
            }
            data_[index] = val;
            size_ += 1;
        }

        VectorIterator<T> end() {
            return VectorIterator<T>(data_ + size_);
        }

        VectorIterator<T> begin() {
            return VectorIterator<T>(data_);
        }
    };
};

#endif