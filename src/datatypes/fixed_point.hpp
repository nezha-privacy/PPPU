#pragma once

#include <bit>
#include <stdexcept>
#include <cmath>

#include "fixed_point.h"

#include "tools/float_parse.hpp"

/************* conversion to/from builtin floating type ***********/

/// @brief Conversion from builtin float/double.
/// @note Allow implicit conversion for convenience.
/// @param val Float/double input
template <size_t N, size_t D>
template <std::floating_point T>
FixedPoint<N, D>::FixedPoint(T val)
{

    // T_max_val is the largest integer value of FixedPoint<N,D>
    constexpr T fix_max_val = std::pow(T{2}, N - D - 1);
    // T_min_val is the least decimal value of FixedPoint<N,D>
    constexpr T fix_min_val = std::pow(T{2}, -int(D));

    if (std::abs(val) > fix_max_val || val == std::numeric_limits<T>::infinity()) {
        *this = FixedPoint<N, D>::infinity();
        return;
    }
    if ( std::abs(val) < fix_min_val ) {
        _data = 0;
        return;
    }
    if ( val == std::numeric_limits<T>::quiet_NaN() ) {
        *this = FixedPoint<N, D>::quiet_NaN();
        return;
    }

    using parse = FloatParse<T>;
    auto e = parse::exponent(val);
    auto f = parse::significand(val);

    auto E = e ? (e - parse::bias) : (1 - parse::bias);
    auto F = e ? (f | parse::unit_mask) : f;

    using FFType = SignedZ2< std::max(N, sizeof(T)*8) >;
    FFType FF = (val >= 0) ? F : -F;

    auto shift = E - decltype(E)(parse::sigd_bits) + decltype(E)(D);

    _data = decltype(_data){ (shift < 0) ? (FF >> -shift) : (FF << shift) };
}

/// @brief Conversion to builtin float/double.
template <size_t N, size_t D>
template <std::floating_point T>
FixedPoint<N, D>::operator T() const
{
    if constexpr ( detail::small<N> )
    {
        using underlying_value_type = typename underlying_type::value_type;
        auto underlying_value = static_cast<underlying_value_type>( this->underlying() );
        return static_cast<T>(underlying_value) / std::pow(T{2}, D);
    }
    else
    {

    // make sure all significand bits can be store in a single limb
    static_assert(sizeof(mp_limb_t) * 8 >= FloatParse<T>::sigd_bits);

    // huge cost here, wonder why
    // replacing Z2<K>::operator== with simple comparison does not help much
    if (_data == underlying_type::zero())
        return T{0};
    if (*this == FixedPoint<N, D>::infinity())
        return std::numeric_limits<T>::infinity();
    if (*this == FixedPoint<N, D>::quiet_NaN())
        return std::numeric_limits<T>::quiet_NaN();

    // get absolute value of the underlying integer representation
    auto &x = _data;
    auto y = abs(x);
    auto data = y.data();

    // find the most significant non-zero limb
    int sgn_limb = underlying_type::N_LIMBS - 1;
    while (sgn_limb > 0 && (data[sgn_limb] == 0))
        sgn_limb--;

    // all bits are zero
    if (data[sgn_limb] == 0)
        return T{0};

    // remove the leading zero before the most sigficant bit
    int exp = sgn_limb * mp_bits_per_limb - int(D);
    int cntlz = std::countl_zero(data[sgn_limb]);
    if (cntlz > 0 && sgn_limb > 0) {
        mpn_lshift(data + sgn_limb - 1, data + sgn_limb - 1, 2, cntlz);
        exp -= cntlz;
    }

    auto s = (x.msb() ? T(-1) : T(1));
    // only calculate one limb
    auto t = static_cast<T>(data[sgn_limb]);
    auto f = std::pow(T{2}, exp);
    return s * t * f;

    }
}

/************************ conversion to/from gmp mpf_class ************************/

/// @brief Conversion to/from gmp mpf_class.
/// @note Calculate the highest non-0 mp_bits_per_limb bits.
template <size_t N, size_t D>
FixedPoint<N, D>::FixedPoint(mpf_class const &val)
{
    if (val >= 0)
        _data = underlying_type(mpz_class(val * std::pow(2, D)));
    else
        _data = underlying_type(-mpz_class(abs(val) * std::pow(2, D)));
}

template <size_t N, size_t D>
mpf_class FixedPoint<N, D>::to_mpf() const
{
    mpf_class ans = mpf_class(abs(_data).to_mpz()) / std::pow(2, D);
    if (_data.msb())
        ans = -ans;
    return ans;
}

/************************  conversion to/from std::string ************************/

/// @brief Conversion to/from std::string( default decimal ).
template <size_t N, size_t D>
FixedPoint<N, D>::FixedPoint(std::string const &str, int base)
{
    throw std::runtime_error("not implemented");
}

template <size_t N, size_t D>
std::string FixedPoint<N, D>::to_string(int base) const
{
    throw std::runtime_error("not implemented");
}

/************************  arithmetic operations ************************/

template <size_t N, size_t D>
FixedPoint<N, D> FixedPoint<N, D>::operator-() const
{
    return FixedPoint<N, D>(-_data);
}

template <size_t N, size_t D>
FixedPoint<N, D> FixedPoint<N, D>::operator+(const FixedPoint<N, D> &rhs) const
{
    return FixedPoint<N, D>(_data + rhs._data);
}

template <size_t N, size_t D>
FixedPoint<N, D> FixedPoint<N, D>::operator-(const FixedPoint<N, D> &rhs) const
{
    return FixedPoint<N, D>(_data - rhs._data);
}

template <size_t N, size_t D>
FixedPoint<N, D> FixedPoint<N, D>::operator*(const FixedPoint<N, D> &rhs) const
{
    FixedPoint<N, D> ans( (_data * rhs._data) >> D);
    return ans;
}

template <size_t N, size_t D>
FixedPoint<N, D> &FixedPoint<N, D>::operator+=(const FixedPoint<N, D> &rhs)
{
    _data += rhs._data;
    return *this;
}

template <size_t N, size_t D>
FixedPoint<N, D> &FixedPoint<N, D>::operator-=(const FixedPoint<N, D> &rhs)
{
    _data -= rhs._data;
    return *this;
}

template <size_t N, size_t D>
FixedPoint<N, D> &FixedPoint<N, D>::operator*=(const FixedPoint<N, D> &rhs)
{
    _data *= rhs._data;
    _data >>= D;
    return *this;
}

/************************ comparison ************************/

template <size_t N, size_t D>
bool FixedPoint<N, D>::operator==(const FixedPoint &rhs) const
{
    return _data == rhs._data;
}

/// @brief Compare this with input.
/// @return if *this < param, return std::strong_ordering::less
///         if *this = param, return std::strong_ordering::equal
///         if *this > param, return std::strong_ordering::greater
template <size_t N, size_t D>
std::strong_ordering FixedPoint<N, D>::operator<=>(const FixedPoint &rhs) const
{
    return _data <=> rhs._data;
}
