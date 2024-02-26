#pragma once

#include <cstdlib>

/// @class BitReference
/// @brief Support operation like { Z2<K, S> ans(0); ans.bit(K - 1) = 1; }.
template <typename T>
class BitReference
{
protected:
    /// @note A Pointer to T, can't point to T array
    T* _ptr;
    std::size_t _bitpos;

    /// @brief Generates a mask with only bitpos 1.
    /// @param flag Flag value. The default value is 1
    /// @return The generated mask
    T mask1(T flag = 1) const { return flag << _bitpos; }
    /// @brief Generates a mask with only bitpos 0.
    /// @return The generated mask
    T mask()            const { return ~mask1(); }
    T value()           const { return *_ptr; }

public:
    BitReference(T* ptr, std::size_t bitpos): _ptr(ptr), _bitpos(bitpos) {}
    BitReference(BitReference const&) = default;

    /// @brief Type conversion operator converts a bit reference to a bool value.
    /// @return Bool value of bit references
    operator bool() const {
        return (mask1() & value()) != 0;
    }

    /// @brief Assignment operator, which assigns a bool value to a bit reference.
    /// @param flag Bool value to be assigned
    /// @return Refers to itself
    BitReference operator=(bool flag) {
        *_ptr &= mask();// set the bitpos bit of the ptr to 0
        *_ptr |= mask1(flag);// set the bitpos bit of the ptr to flag
        return *this;
    }
};
