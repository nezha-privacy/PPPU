#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream> // for debug
#include <stdexcept>

#include "byte_vector.h"

using value_type = ByteVector::value_type;
using size_type = ByteVector::size_type;
using reference = ByteVector::reference;
using const_reference = ByteVector::const_reference;
using pointer = ByteVector::pointer;
using const_pointer = ByteVector::const_pointer;

/************************ constructor ************************/

ByteVector::ByteVector(size_type n)
    : _vec(n)
{
}

ByteVector::ByteVector(size_type n, const_reference val)
    : _vec(n, val)
{
}

ByteVector::ByteVector(const void *ptr, size_type n)
    : _vec(n)
{
    memcpy(this->data(), ptr, n);
}

ByteVector::ByteVector(std::initializer_list<value_type> list)
    : _vec(list)
{
}

/************************ data access ************************/

/// @return The reference of element accessed
reference ByteVector::at(size_type pos)
{
    return _vec.at(pos);
}

const_reference ByteVector::at(size_type pos) const
{
    return _vec.at(pos);
}

reference ByteVector::operator[](size_type pos)
{
    return _vec[pos];
}

const_reference ByteVector::operator[](size_type pos) const
{
    return _vec[pos];
}

/// @return The pointer of underlying data
value_type *ByteVector::data()
{
    return _vec.data();
}

const value_type *ByteVector::data() const
{
    return _vec.data();
}

/************************ capacity ************************/

/// @brief Determine whether the byte vector is empty, return true when it is empty.
/// @return True when it is empty
bool ByteVector::empty() const
{
    return _vec.empty();
}

/// @brief Get the size of underlying RawVector, which means size of data has stored.
/// @return The size_type of underlying RawVector
size_type ByteVector::size() const
{
    return _vec.size();
}

/// @brief Get the capacity of underlying RawVector, which means size able to store.
/// @return The capacity of underlying RawVector
size_type ByteVector::capacity() const
{
    return _vec.capacity();
}

/// @brief Redefine the size of underlying RawVector with n.
/// @param n Input size of size_type of underlying RawVector
void ByteVector::reserve(size_type n)
{
    _vec.reserve(n);
}

/// @brief Allocate appropriate memory space to the raw vector.
void ByteVector::shrink_to_fit()
{
    _vec.shrink_to_fit();
}

/// @brief Open up the memory space required for a byte vector with size n.
/// @param n Input size of vector
void ByteVector::resize(size_type n)
{
    _vec.resize(n);
}

/// @brief Clear the underlying RawVector, but keep the capacity of it.
/// @note Set size of underlying RawVector to zero, leave capacity unchanged
void ByteVector::clear()
{
    _vec.clear();
}

/************************ operations ************************/

/// @brief Put a new element to the end of the vector with a const reference input.
/// @param val The const reference input
void ByteVector::push_back(const_reference val)
{
    auto pos = this->size();
    this->resize(this->size() + 1);
    _vec[pos] = val;
}

/// @brief Put a new element to the end of the vector with a pointer of input of size n.
/// @param ptr The pointer of input
/// @param n The size of input vector
void ByteVector::push_back(const void *ptr, size_type n)
{
    if (n<=0) return;
    if (ptr == nullptr)
        throw std::invalid_argument("nullptr");

    auto pos = this->size();
    this->resize(this->size() + n);
    memcpy(this->data() + pos, ptr, n);
}

/// @brief Delete a element at the end of the vector.
void ByteVector::pop_back()
{
    if (this->size() == 0)
        throw std::out_of_range("");

    this->resize(this->size() - 1);
}

/// @brief Delete n elements at the end of the vector.
void ByteVector::pop_back(size_type n)
{
    if (n > this->size())
        throw std::out_of_range("");

    this->resize(this->size() - n);
}

/// @brief Copy a exist ByteVector to construct a new ByteVector.
/// @return A ByteVector same as the exist one.
ByteVector ByteVector::copy() const
{
    return ByteVector(this->data(), this->size());
}

bool ByteVector::operator==(ByteVector const& other) const
{
    if(this->size() != other.size())
        return false;

    for(std::size_t i = 0; i < this->size(); ++i) {
        if( this->operator[](i) != other[i] ) {
            return false;
        }
    }

    return true;
}
