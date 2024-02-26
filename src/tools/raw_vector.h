#pragma once

#include <cstdlib>
#include <type_traits>
#include <vector>

/// @class RawVector
/// @brief Raw vector is used in construction of bit vector and byte vector.
/// @note Raw vector does not call objects constructors when allocating memories, so only POD Type is allowed
///       Raw vector is faster to construct, destroy and resize.
/// @tparam T The value type which the vector contains in
template <typename T>
requires std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>
class RawVector
{
public:
    using size_type = std::size_t;
    
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using pointer         = value_type*;
    using const_pointer   = value_type const*;

protected:
    size_type               _size;  // actual size
    std::vector<value_type> _vec;

public:
    ~RawVector() = default;

    /// @brief Default constructor with size 0.
    RawVector();
    RawVector(RawVector&&);
    RawVector(RawVector const&);
    RawVector& operator=(RawVector&&);
    RawVector& operator=(RawVector const&);

    /// @brief Construct with preset size, such as bits per limb.
    /// @param size_type Input size type
    RawVector(size_type);

    /// @brief Construct with preset size and type T values.
    /// @param size_type Input size type
    /// @param T Input type T values
    RawVector(size_type, T const&);

    /// @brief Construct with type T list.
    /// @param T Input type T list
    RawVector(std::initializer_list<T>);

    /// @brief Redefine the size of data has stored with n.
    /// @param n Input size of size_type
    void resize (size_type n);

    /// @brief Open up the memory space required for a raw vector of type T with size n.
    /// @param n Input size of vector
    void reserve(size_type n);

    /// @brief Determine whether the raw vector is empty, return true when it is empty.
    /// @return True when it is empty
    bool      empty()    const;

    /// @brief Get the preset size type, which means size of data has stored.
    /// @return The size_type
    size_type size()     const;

    /// @brief Get the capacity of vector, which means size able to store.
    /// @return The capacity of _vec
    size_type capacity() const;

    /// @brief Allocate appropriate memory space to the raw vector.
    /// @note May reallocate memory
    void shrink_to_fit();

    /// @brief Clear the vector, but keep the capacity of it.
    /// @note Set size to zero, leave capacity unchanged
    void clear();

    /// @return The pointer of underlying data
    pointer       data();
    const_pointer data() const;

    /// @return The reference of underlying data at specify position
    reference       operator[](size_type pos);
    const_reference operator[](size_type pos) const;

    reference       at(size_type pos);
    const_reference at(size_type pos) const;

};
