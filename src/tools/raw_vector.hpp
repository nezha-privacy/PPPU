#pragma once

#include <cstring>
#include <stdexcept>

#include "raw_vector.h"

/************************ constructor ************************/

/// @brief Default constructor with size 0.
template <typename T>
RawVector<T>::RawVector()
    : _size(0)
{
}

template <typename T>
RawVector<T>::RawVector(RawVector<T> &&other)
    : _size(other._size), _vec(std::move(other._vec))
{
    other._size = 0;
}

template <typename T>
RawVector<T>::RawVector(RawVector<T> const &other)
{
    auto new_size = other.size();
    _size = new_size;
    _vec.reserve(new_size);
    if(new_size > 0)
        memcpy(this->data(), other.data(), new_size * sizeof(T));
}

template <typename T>
RawVector<T> &RawVector<T>::operator=(RawVector<T> &&other)
{
    _size = other._size;
    _vec = std::move(other._vec);
    other._size = 0;
    return *this;
}

template <typename T>
RawVector<T> &RawVector<T>::operator=(RawVector<T> const &other)
{
    // do not use vector's copy assignment because it
    // does not copy anything when its size is 0
    auto new_size = other.size();
    _size = new_size;
    _vec.reserve(new_size);  // no need to save original contents
    if(new_size > 0)
        memcpy(this->data(), other.data(), new_size * sizeof(T));
    return *this;
}

/// @brief Construct with preset size, such as bits per limb.
/// @param size_type Input size type
template <typename T>
RawVector<T>::RawVector(size_type n)
    : _size(0)
{
    this->resize(n);
}

/// @brief Construct with preset size and type T values.
/// @param size_type Input size type
/// @param T Input type T values
template <typename T>
RawVector<T>::RawVector(size_type n, T const& val)
    : _size(0), _vec(n, val)
{
}

/// @brief Construct with type T list.
/// @param T Input type T list
template <typename T>
RawVector<T>::RawVector(std::initializer_list<T> list)
    : _size(list.size()), _vec(list)
{
}

/************************ capacity ************************/

/// @brief Determine whether the raw vector is empty, return true it is empty.
/// @return True when it is empty
template <typename T>
bool RawVector<T>::empty() const
{
    return _size == 0;
}

/// @brief Get the preset size type, which means size of data has stored.
/// @return The size_type
template <typename T>
RawVector<T>::size_type RawVector<T>::size() const
{
    return _size;
}

/// @brief Get the capacity of vector, which means size able to store.
/// @return The capacity of _vec
template <typename T>
RawVector<T>::size_type RawVector<T>::capacity() const
{
    return _vec.capacity();
}

/// @brief Redefine the size of data has stored with n.
/// @param n Input size of size_type
template <typename T>
void RawVector<T>::resize(size_type n)
{
    size_type old_size = _size;
    size_type new_size = n;

    if (new_size <= old_size) {
        _size = new_size;
        return;
    }
    // save original contents first, in case of reallocation
    this->reserve(new_size);
    _size = new_size;
}

/// @brief Open up the memory space required for a raw vector of type T with size n.
/// @param n Input size of vector
template <typename T>
void RawVector<T>::reserve(size_type n)
{
    // save original contents first, in case of reallocation
    size_type new_capacity = n;
    size_type old_capacity = this->capacity();
    size_type size = _size;

    if (new_capacity <= old_capacity)
        return;

    std::vector<T> tmp;
    tmp.reserve(new_capacity);
    if (size > 0)
        memcpy(tmp.data(), this->data(), size);
    _vec.swap(tmp);
}

/// @brief Allocate appropriate memory space to the raw vector.
/// @note May reallocate memory
template <typename T>
void RawVector<T>::shrink_to_fit()
{
    if (this->size() == this->capacity())
        return;

    // save original contents first, in case of reallocation
    std::vector<T> tmp;
    tmp.reserve(this->size());

    // reserve may allocate more memory than required
    // so if that happens, return immediately
    if (tmp.capacity() >= this->capacity())
        return;

    if (this->size() > 0)
        memcpy(tmp.data(), this->data(), this->size());
    _vec.swap(tmp);
}

/// @brief Clear the vector, but keep the capacity of it.
/// @note Set size to zero, leave capacity unchanged
template <typename T>
void RawVector<T>::clear()
{
    _size = 0;
}

/************************ data access ************************/

/// @return The pointer of underlying data
template <typename T>
RawVector<T>::pointer RawVector<T>::data()
{
    return _vec.data();
}

template <typename T>
RawVector<T>::const_pointer RawVector<T>::data() const
{
    return _vec.data();
}

/// @return The reference of underlying data at specify position
template <typename T>
RawVector<T>::reference RawVector<T>::operator[](size_type pos)
{
    return this->data()[pos];
}

template <typename T>
RawVector<T>::const_reference RawVector<T>::operator[](size_type pos) const
{
    return this->data()[pos];
}

template <typename T>
RawVector<T>::reference RawVector<T>::at(size_type pos)
{
    if (pos > this->size())
        throw std::out_of_range("");

    return this->operator[](pos);
}

template <typename T>
RawVector<T>::const_reference RawVector<T>::at(size_type pos) const
{
    if (pos > this->size())
        throw std::out_of_range("");

    return this->operator[](pos);
}
