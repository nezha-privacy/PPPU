#pragma once

#include <cassert>
#include <string>
#include <bit>
#include <type_traits>
#include <cmath>
#include <limits>

#include <stdexcept>

#include "Z2k.h"

#include "tools/float_parse.hpp"

/************************ small Z2K ************************/

/// @brief Set extra bits to 0.
/// @note if K is 1, 8, 16, 32, 64 or 128 then this does not introduce any cost.
template <size_t K, bool S>
requires detail::small<K>
void Z2<K, S>::_normalize()
{
    if constexpr (K == 1 || K == 8 || K == 16 || K == 32 || K == 64 || K == 128) {
        /* do nothing */
    } else {
        _data &= _mask;
    }
}

/// @brief Converts a finite field element to a value_type representation.
template <size_t K, bool S>
requires detail::small<K>
Z2<K, S>::operator Z2<K, S>::value_type() const
{
    if constexpr (K == 1) {
        return _data;
    } else if constexpr (S) {
        // do sign extension
        constexpr auto cnt = sizeof(value_type) * 8 - K;
        return value_type(value_type(_data << cnt) >> cnt);
    } else {
        return _data;
    }
}

/// @brief Converts a finite field element to a int type representation.
template <size_t K, bool S> requires detail::small<K>
template <typename Int>     requires std::integral<Int>
Z2<K, S>::operator Int() const
{
    return Int(value_type(*this));
}

/// @brief Converts a finite field element to a double type representation.
template <size_t K, bool S>
requires detail::small<K>
template <std::floating_point T>
Z2<K, S>::operator T() const
{
    return T(value_type(*this));
}

/// @brief Explicit conversion for different K.
template <size_t K1, bool S1>
requires detail::small<K1>
template <size_t K2, bool S2>
requires detail::different<K1, K2> || detail::different_bool<S1, S2>
Z2<K1, S1>::Z2(Z2<K2, S2> const &other)
{
    if constexpr (detail::small<K2>) {
        // convert to other value_type
        auto other_val = (typename Z2<K2, S2>::value_type)(other);
        // convert to this value_type
        auto this_val = value_type(other_val);
        // convert to this underlying representation
        *this = this_val;
    } else {
        // convert to this value_type
        auto this_val = value_type(other.data()[0]);
        // convert to this underlying representation
        *this = this_val;
    }
}

template<typename T>
std::string number_to_string(T value) {
    if (value == 0) {
        return "0";
    }
    bool negative = value < 0;
    if (negative) {
        value = -value;
    }

    std::string result;
    while (value > 0) {
        result += '0' + static_cast<int>(value % 10);
        value /= 10;
    }

    if (negative) {
        result += '-';
    }

    std::reverse(result.begin(), result.end());
    return result;
}
/// @brief Converts a finite field element to a string representation(default decimal).
template <size_t K, bool S>
requires detail::small<K>
std::string Z2<K, S>::to_string() const
{
     if constexpr (K == 1)
    {
        std::string str = "" + std::to_string(*(uint8_t*)(this->data()));
        return str;
    }
    else
    {
        std::string str = "" + number_to_string<value_type>(*(value_type*)(this->data()));
        return str;
    }
}

/// @brief Converts a finite field element to a string representation.
template <size_t K, bool S>
requires detail::small<K>
std::string Z2<K, S>::to_string(int base) const
{
    if constexpr ( K <= 64 )
        return mpz_class(value_type(*this)).get_str(base);
    else
        throw std::runtime_error("not implemented");
}

/// @brief Get the value of the bit at the most significant position in the _data.
template <size_t K, bool S>
requires detail::small<K>
bool Z2<K, S>::msb() const
{
    return 1 & (_data >> (K - 1));
}

/// @brief Get the value of the bit at the specified position in the _data.
template <size_t K, bool S>
requires detail::small<K>
bool Z2<K, S>::bit(size_t index) const
{
    return 1 & (_data >> index);
}

/// @brief Support operation like { Z2<K, S> ans(0); ans.bit(K - 1) = 1; }
template <size_t K, bool S>
requires detail::small<K>
    BitReference<typename Z2<K, S>::value_type>
    Z2<K, S>::bit(size_t index)
{
    return BitReference(&_data, index);
}

template <size_t K, bool S>
requires detail::small<K>
    Z2<K, S> Z2<K, S>::operator>>(size_t rhs) const
{
    constexpr size_t cnt = sizeof(_data) * 8 - K;
    return Z2(value_type(value_type(_data << cnt) >> (cnt + rhs)));
}

template <size_t K, bool S>
requires detail::small<K>
Z2<K, S> &Z2<K, S>::operator>>=(size_t rhs)
{
    constexpr size_t cnt = sizeof(_data) * 8 - K;
    _data = value_type(value_type(_data << cnt) >> (cnt + rhs));
    _normalize();
    return *this;
}

/************************ large Z2K ************************/

/************************ constructor ************************/

template <size_t K, bool S>
requires detail::large<K>
constexpr Z2<K, S> Z2<K, S>::min()
{
    Z2<K, S> ans(0);
    if constexpr (S)
        ans.bit(K - 1) = 1;
    return ans;
}

template <size_t K, bool S>
requires detail::large<K>
constexpr Z2<K, S> Z2<K, S>::max()
{
    Z2<K, S> ans(-1);
    if constexpr (S)
        ans.bit(K - 1) = 0;
    return ans;
}

/// @brief Explicit conversion for different K.
template <size_t K1, bool S1>
requires detail::large<K1>
template <size_t K2, bool S2>
requires detail::different<K1, K2> || detail::different_bool<S1, S2>
    Z2<K1, S1>::Z2(Z2<K2, S2> const &other)
{

    // copy contents first
    auto copy_size = std::min(this->size_in_bytes(), other.size_in_bytes());
    memcpy(_data, other.data(), copy_size);

    // zero/sign extension next
    if constexpr (K1 > K2) {
        if constexpr (S2) {
            detail::mpx2k_sign_extension<K1, K2>(_data);
        } else {
            detail::mpx2k_zero_extension<K1, K2>(_data);
        }
    } else if constexpr (K1 < K2) {
        detail::mpx2k_norm<K1>(_data);
    }
}

/// @note If K is 8, 16, 32, and multiple of 64, then this does not introduce any cost.
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>::Z2()
{
    detail::mpx2k_norm<K>(_data);
}

/// @note Allow implicit conversion for convenience.
template <size_t K, bool S>
requires detail::large<K>
template <std::integral Int>
    Z2<K, S>::Z2(Int val)
{
    this->assign(val);
}

template <size_t K, bool S>
requires detail::large<K>
template <std::floating_point Float>
    Z2<K, S>::Z2(Float val)
{
    this->assign(val);
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>::Z2(mpz_class const &val)
{
    this->assign(val);
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>::Z2(std::string const &str, int base)
{
    this->assign(str, base);
}

/************************ export ************************/

template <size_t K, bool S>
requires detail::large<K>
template <typename Int>
requires (std::integral<Int> && sizeof(Int) <= sizeof(mp_limb_t))
Z2<K, S>::operator Int() const
{
    return static_cast<Int>( this->_data[0] );
}

template <size_t K, bool S>
requires detail::large<K>
Z2<K, S>::operator double() const
{
    return this->to_mpz().get_d();
}

/// @brief Converts a finite field element to a string representation.
template <size_t K, bool S>
requires detail::large<K>
std::string Z2<K, S>::to_string(int base) const
{
    return this->to_mpz().get_str(base);
}

template <size_t K, bool S>
requires detail::large<K>
mpz_class Z2<K, S>::to_mpz() const
{
    mpz_class ans;

    mpz_import(
        ans.get_mpz_t(),
        N_LIMBS,
        -1,
        sizeof(limb_t),
        0, 0,
        _data);

    // for signed negative Z2K, the value imported by mpz is in fact
    // 2^K - abs(*this), so we need to subtract it by 2^K to get the
    // actual value
    if (S && this->msb() == 1) {
        ans -= (mpz_class(1) << K);
    }

    return ans;
}

/************************ import ************************/

// template <size_t K, bool S>
// requires detail::large<K>
// Z2<K, S>::Z2(double val)
// {
//     // TODO: implement this
//     this->assign(long(val));
// }

template <size_t K, bool Signed>
requires detail::large<K>
template <std::integral Int>
void Z2<K, Signed>::assign(Int val)
{
    static_assert( sizeof(Int) <= sizeof(limb_t) );

    _data[0] = static_cast<limb_t>(val);

    if constexpr (std::is_signed_v<Int>)
        detail::mpx2k_sign_extension<K, sizeof(Int) * 8>(_data);
    else
        detail::mpx2k_zero_extension<K, sizeof(Int) * 8>(_data);
}

// when val is Inf or NaN, the result is undefined
template <size_t K, bool Signed>
requires detail::large<K>
template <std::floating_point Float>
void Z2<K, Signed>::assign(Float val)
{
    if(std::abs(val) < 1)
    {
        this->assign(0);
    }

    // |x| = f * 2^e
    auto [s, e, f] = FloatParse<Float>::parse(val);

    // sf = f * (-1)^s
    auto sf = (s ? -f : f);

    if(e <= 0)
    {
        this->assign( sf >> (-e) );
    }
    else
    {
        this->assign( sf );
        this->operator<<=( static_cast<std::size_t>(e) );
    }
}

template <size_t K, bool S>
requires detail::large<K>
void Z2<K, S>::assign(mpz_class const &val)
{

    bool negative = (mpz_sgn(val.get_mpz_t()) < 0);
    bool too_large = (mpz_sizeinbase(val.get_mpz_t(), 2) > K);

    mpz_t true_val;
    mpz_init(true_val);
    if (negative || too_large) {
        mpz_mod_2exp(true_val, val.get_mpz_t(), K);
    } else {
        mpz_set(true_val, val.get_mpz_t());
    }

    std::size_t count;
    mpz_export(
        _data,
        &count,
        -1,
        sizeof(limb_t),
        0, 0,
        true_val);

    if (count > N_LIMBS)
        throw std::runtime_error("Z2K assign error");

    mpn_zero(_data + count, N_LIMBS - count);
}

template <size_t K, bool S>
requires detail::large<K>
void Z2<K, S>::assign(std::string const &str, int base)
{
    mpz_class mpz_val(str, base);
    this->assign(mpz_val);
}

/************************ arithmetic operations ************************/

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator-() const
{
    Z2<K, S> ans;
    detail::mpx2k_neg<K>(ans._data, _data);
    return ans;
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator+(const Z2<K, S> &rhs) const
{
    Z2<K, S> ans;
    detail::mpx2k_add<K>(ans._data, _data, rhs._data);
    return ans;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator-(const Z2<K, S> &rhs) const
{
    Z2<K, S> ans;
    detail::mpx2k_sub<K>(ans._data, _data, rhs._data);
    return ans;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator*(const Z2<K, S> &rhs) const
{
    Z2<K, S> ans;
    detail::mpx2k_mul<K>(ans._data, _data, rhs._data);
    return ans;
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>
&Z2<K, S>::operator+=(const Z2<K, S> &rhs)
{
    detail::mpx2k_add<K>(_data, _data, rhs._data);
    return *this;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>
&Z2<K, S>::operator-=(const Z2<K, S> &rhs)
{
    detail::mpx2k_sub<K>(_data, _data, rhs._data);
    return *this;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>
&Z2<K, S>::operator*=(const Z2<K, S> &rhs)
{
    detail::mpx2k_mul<K>(_data, _data, rhs._data);
    return *this;
}

/************************ bitwise operations ************************/

/// @brief Get the value of the bit at the most significant position in the _data.
template <size_t K, bool S>
requires detail::large<K>
bool Z2<K, S>::msb() const
{
    return this->bit(K - 1);
}

/// @brief Get the value of the bit at the specified position in the _data.
template <size_t K, bool S>
requires detail::large<K>
bool Z2<K, S>::bit(size_type pos) const
{
    assert(pos < K);
    size_type limb_pos = pos / N_BITS_PER_LIMB;
    size_type bit_pos = pos % N_BITS_PER_LIMB;
    return 1 & (_data[limb_pos] >> bit_pos);
}

/// @brief Support operation like { Z2<K, S> ans(0); ans.bit(K - 1) = 1; }.
template <size_t K, bool S>
requires detail::large<K>
    BitReference<typename Z2<K, S>::limb_t> Z2<K, S>::bit(size_type pos)
{
    assert(pos < K);
    size_type limb_pos = pos / N_BITS_PER_LIMB;
    size_type bit_pos = pos % N_BITS_PER_LIMB;
    return BitReference<limb_t>(_data + limb_pos, bit_pos);
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator&(const Z2<K, S> &rhs) const
{
    Z2<K, S> ans;
    detail::mpx2k_and<K>(ans._data, _data, rhs._data);
    return ans;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator|(const Z2<K, S> &rhs) const
{
    Z2<K, S> ans;
    detail::mpx2k_ior<K>(ans._data, _data, rhs._data);
    return ans;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator^(const Z2<K, S> &rhs) const
{
    Z2<K, S> ans;
    detail::mpx2k_xor<K>(ans._data, _data, rhs._data);
    return ans;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator~() const
{
    Z2<K, S> ans;
    detail::mpx2k_com<K>(ans._data, _data);
    return ans;
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>
&Z2<K, S>::operator&=(const Z2<K, S> &rhs)
{
    detail::mpx2k_and<K>(_data, _data, rhs._data);
    return *this;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>
&Z2<K, S>::operator|=(const Z2<K, S> &rhs)
{
    detail::mpx2k_ior<K>(_data, _data, rhs._data);
    return *this;
}
template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>
&Z2<K, S>::operator^=(const Z2<K, S> &rhs)
{
    detail::mpx2k_xor<K>(_data, _data, rhs._data);
    return *this;
}

/************************ shift operations ************************/

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator<<(size_type cnt) const
{
    Z2<K, S> ans;
    detail::mpx2k_lshift<K>(ans._data, _data, cnt);
    return ans;
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S> Z2<K, S>::operator>>(size_type cnt) const
{
    Z2<K, S> ans;
    detail::mpx2k_rshift<K, S>(ans._data, _data, cnt);
    return ans;
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>
&Z2<K, S>::operator<<=(size_type cnt)
{
    detail::mpx2k_lshift<K>(_data, _data, cnt);
    return *this;
}

template <size_t K, bool S>
requires detail::large<K>
    Z2<K, S>
&Z2<K, S>::operator>>=(size_type cnt)
{
    detail::mpx2k_rshift<K, S>(_data, _data, cnt);
    return *this;
}

/************************ comparison ************************/

template <size_t K, bool S>
requires detail::large<K>
bool Z2<K, S>::operator==(const Z2<K, S> &rhs) const
{
    return detail::mpx2k_cmp<K, S>(_data, rhs._data) == 0;
}

template <size_t K, bool S>
requires detail::large<K>
std::strong_ordering Z2<K, S>::operator<=>(const Z2<K, S> &rhs) const
{
    auto cmp = detail::mpx2k_cmp<K, S>(_data, rhs._data);
    if (cmp < 0)
        return std::strong_ordering::less;
    else if (cmp == 0)
        return std::strong_ordering::equal;
    else
        return std::strong_ordering::greater;
}

/************************ math ************************/

/// @brief Calculate the multiplicative inverse of a given unsigned binary integer.
template <size_t K>
UnsignedZ2<K> inv(UnsignedZ2<K> const &x)
{
    assert(x.bit(0) == 1);

    UnsignedZ2<K> ans = 1;
    for (size_t i = 0; i < K; ++i) {
        ans += UnsignedZ2<K>(1 - (x * ans).bit(i)) << i;
    }
    return ans;
}

/// @brief Calculate the square root of a given unsigned binary integer.
template <size_t K>
UnsignedZ2<K> sqrt(UnsignedZ2<K> const &x)
{
    // x % 8 == 1
    assert(x.bit(0) == 1);
    assert(x.bit(1) == 0);
    assert(x.bit(2) == 0);

    UnsignedZ2<K> ans = 1;
    for (size_t i = 0; i < K - 1; ++i) {
        ans += UnsignedZ2<K>((x - ans * ans).bit(i + 1)) << i;
    }
    return ans;
}

/// @brief Calculate the absolute value of a given binary integer.
template <size_t K, bool S>
Z2<K, S> abs(Z2<K, S> const& x) {
    if constexpr ( S == false ) {
        return x;
    } else {
        return x.msb() ? -x : x;
    }
}

/// @brief Count the number of consecutive zeros starting from the significant bit-by-bit.
template <size_t K, bool S>
int countl_zero(Z2<K, S> const& x) {
    if constexpr ( detail::small<K> ) {
        return std::countl_zero( *x.data() );
    }

    constexpr int bits_per_limb = decltype(x)::N_BITS_PER_LIMB;

    int i = x.size_in_limbs() - 1;

    // find the first non zero limb
    auto data = x.data();
    while(i > 0 && data[i] == 0)    i--;

    int prev = (x.size_in_limbs() - i - 1) * bits_per_limb;
    int last = std::countl_zero(data[i]);
    int ans  = prev + last;

    return ans;
}
