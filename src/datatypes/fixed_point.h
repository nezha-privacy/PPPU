#pragma once

#include <cstdlib>
#include <compare>
#include <cmath>
#include <concepts>
#include <numeric>

#include <gmp.h>
#include <gmpxx.h>

#include "concepts.h"
#include "Z2k.hpp"

template <std::size_t N, std::size_t D>
class FixedPoint {

protected:
    using underlying_type = SignedZ2<N>;

    /// @brief Use SignedZ2<N> as the underlying data type
    underlying_type _data;

    /// @note D is not considered when assigned with SignedZ2<N>
    explicit FixedPoint(underlying_type data): _data(data) {}

public:

    static constexpr bool trivially_serializable = true;

    static constexpr FixedPoint min()        { return FixedPoint( underlying_type::one()   ); }
    static constexpr FixedPoint lowest()     { return FixedPoint( underlying_type::min()   ); }
    static constexpr FixedPoint max()        { return FixedPoint( underlying_type::max() -2); }
    static constexpr FixedPoint epsilon()    { return FixedPoint( underlying_type::one()   ); }
    /// @brief When assigned to std::numeric_limits<T>::infinity(), 
    ///        or a value whose absolute value is greater than 2^(N-D-1).
    static constexpr FixedPoint infinity()   { return FixedPoint( underlying_type::max()   ); }
    /// @brief When assigned to std::numeric_limits<T>::quiet_NaN().
    static constexpr FixedPoint quiet_NaN()  { return FixedPoint( underlying_type::max() -1); }

    FixedPoint()                             = default;
    ~FixedPoint()                            = default;
    FixedPoint(FixedPoint&&)                 = default;
    FixedPoint(const FixedPoint&)            = default;
    FixedPoint& operator=(FixedPoint&&)      = default;
    FixedPoint& operator=(const FixedPoint&) = default;

    /// @brief Return the underlying data.
    /// @return The underlying data
    underlying_type& underlying() { return _data; }
    underlying_type const& underlying() const { return _data; }

    /// @brief Conversion from builtin float/double.
    /// @note Allow implicit conversion for convenience.
    /// @param val Float/double input
    template <std::floating_point T>
    FixedPoint(T val);

    /// @brief Conversion to builtin float/double.
    template <std::floating_point T>
    explicit operator T() const;

    /// @brief Conversion to/from gmp mpf_class.
    /// @note Calculate the highest non-0 mp_bits_per_limb bits.
    explicit FixedPoint(mpf_class const& val);
    mpf_class to_mpf() const;

    /// @brief Conversion to/from std::string( default decimal ).
    explicit FixedPoint(std::string const& str, int base = 10);
    std::string to_string(int base = 10) const;

    FixedPoint operator-() const;
    FixedPoint operator+(const FixedPoint& rhs) const;
    FixedPoint operator-(const FixedPoint& rhs) const;
    FixedPoint operator*(const FixedPoint& rhs) const;

    FixedPoint& operator+=(const FixedPoint& rhs);
    FixedPoint& operator-=(const FixedPoint& rhs);
    FixedPoint& operator*=(const FixedPoint& rhs);

    bool                 operator== (const FixedPoint& rhs) const;
    /// @brief Compare this with input.
    /// @return if *this < param, return std::strong_ordering::less
    ///         if *this = param, return std::strong_ordering::equal
    ///         if *this > param, return std::strong_ordering::greater
    std::strong_ordering operator<=>(const FixedPoint& rhs) const;

};
