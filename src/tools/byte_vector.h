#pragma once

#include <vector>
#include <cstddef>
#include <cstdlib>
#include <initializer_list>

#include "raw_vector.hpp"

/// @class ByteVector
/// @brief Base value type used in network communication, which stores data in byte form.
class ByteVector {

protected:
    RawVector<std::byte> _vec;

public:

    using value_type      = std::byte;
    using size_type       = std::size_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;

    /// @note mByteVector stores ByteVector in std::vector, which
    ///       calls ByteVector's copy constructor if it's defined
    ///       this is definitely not desired for large bytevector
    ///       and so it is deleted.
    ByteVector()                             = default;
    ~ByteVector()                            = default;
    ByteVector(ByteVector&&)                 = default;
    ByteVector(const ByteVector&)            = delete;
    ByteVector& operator=(ByteVector&&)      = default;
    ByteVector& operator=(const ByteVector&) = delete;

    explicit ByteVector(size_type);
    ByteVector(size_type, const_reference);
    ByteVector(const void*, size_type);
    ByteVector(std::initializer_list<value_type>);

    /// @return The reference of element accessed
    reference       at(size_type);
    const_reference at(size_type) const;
    reference       operator[] (size_type);
    const_reference operator[] (size_type) const;

    /// @return The pointer of underlying data
    value_type*       data ();
    const value_type* data () const;

    /// @brief Determine whether the byte vector is empty, return true when it is empty.
    /// @return True when it is empty
    bool      empty   () const;

    /// @brief Get the size of underlying RawVector, which means size of data has stored.
    /// @return The size_type of underlying RawVector
    size_type size    () const;

    /// @brief Get the capacity of underlying RawVector, which means size able to store.
    /// @return The capacity of underlying RawVector
    size_type capacity() const;

    /// @brief Allocate appropriate memory space to the raw vector.
    void shrink_to_fit();

    /// @brief Redefine the size of underlying RawVector with n.
    /// @param n Input size of size_type of underlying RawVector
    void reserve(size_type n);

    /// @brief Open up the memory space required for a byte vector with size n.
    /// @param n Input size of vector
    void resize (size_type n);

    /// @brief Clear the underlying RawVector, but keep the capacity of it.
    /// @note Set size of underlying RawVector to zero, leave capacity unchanged
    void clear();

    /// @brief Put a new element to the end of the vector with a const reference input.
    /// @param val The const reference input
    void push_back(const_reference val);

    /// @brief Put a new element to the end of the vector with a pointer of input of size n.
    /// @param ptr The pointer of input
    /// @param n The size of input vector
    void push_back(const void* ptr, size_type n);

    /// @brief Delete a element at the end of the vector.
    void pop_back ();

    /// @brief Delete n elements at the end of the vector.
    void pop_back (size_type n);

    /// @brief Copy a exist ByteVector to construct a new ByteVector.
    /// @return A ByteVector same as the exist one.
    ByteVector copy() const;

    bool operator==(ByteVector const& other) const;
};

using mByteVector = std::vector<ByteVector>;
