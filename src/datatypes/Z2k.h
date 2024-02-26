#pragma once

#include <concepts>
#include <type_traits>
#include <cstdlib>
#include <string>

#include <gmp.h>
#include <gmpxx.h>

#include "int128.h"

#include "mpx2k.hpp"
#include "../tools/bit_reference.h"

namespace detail {
/// @tparam K is a number of bits to represent the modulus 2^K
template <size_t K> concept large = (K > 128);
template <size_t K> concept small = (0 < K && K <= 128);

template <bool x, bool y> concept same_bool      = (x == y);
template <bool x, bool y> concept different_bool = (x != y);

/// @tparam K and S are number of bits to represent the modulus 2^K and 2^S,respectively
template <std::size_t K, std::size_t S> concept same      = (K == S);
template <std::size_t K, std::size_t S> concept different = (K != S);
}

/// @brief Primary template declaration.
/// @details The 'Z2' class represents a finite field whose modulus is 2^K, 
///          and provides basic operations and operations on finite fields.
/// @tparam K is a number of bits to represent the modulus 2^K
/// @tparam Signed is used to determine whether Z2 is a signed number or an unsigned number
template <size_t K, bool Signed> class Z2;

template <size_t K> using SignedZ2   = Z2<K, true>;
template <size_t K> using UnsignedZ2 = Z2<K, false>;

/// @class Z2
/// @brief Template specilization for large K.
template <size_t K, bool Signed> requires detail::large<K>
class Z2<K, Signed> {
public:
    using size_type = std::size_t;
    using limb_t    = mp_limb_t;

    /// @brief Represents the number of bits in a single mp_limb_t data type.
    static constexpr size_type N_BITS_PER_LIMB = GMP_LIMB_BITS;

    /// @brief N_LIMBS calculates the number of limbs required to represent K in bits.
    static constexpr size_type N_LIMBS = ceildiv(K, N_BITS_PER_LIMB);

protected:

    /// @brief A limb_t array that stores finite field elements
    /// @details For an array of type mp_limb_t, the least significant byte 
    ///          is always in front and the native byte order of the host CPU is used.
    /// @note Support positive and negative number, negative number 
    ///       are stored in _data as K-bit(not N_BITS_PER_LIMB * N_LIMBS) complement.
    limb_t _data[N_LIMBS];

public:
    static constexpr bool trivially_serializable = true;

    static constexpr Z2 zero() { return Z2(0); }
    static constexpr Z2 one()  { return Z2(1); }
    static constexpr Z2 min();
    static constexpr Z2 max();

    /// @brief Return the number of limbs required to represent _data.
    static constexpr size_type size_in_limbs() { return N_LIMBS;       }
    /// @brief Return the number of bytes required to represent _data.
    static constexpr size_type size_in_bytes() { return sizeof(_data); }

/************************ ctor, dtor, assign ************************/

    ~Z2()                    = default;
    Z2(Z2&&)                 = default;
    Z2(const Z2&)            = default;
    Z2& operator=(Z2&&)      = default;
    Z2& operator=(const Z2&) = default;

    /// @brief If K is multiple of 64,then this does not introduce any cost.
    Z2();

    /// @brief Allow implicit conversion from builtin types for convenience.
    template <std::integral Int>
    Z2(Int);

    /// @brief Explicit conversion from floating-point types.
    template <std::floating_point Float>
    explicit Z2(Float);

    /// @brief Explicit conversion for different number of bits or different sign.
    template <size_t S, bool OtherSigned>
    requires detail::different<K, S> || detail::different_bool<Signed, OtherSigned>
    explicit Z2(Z2<S, OtherSigned> const& other);

    /// @brief Constructs finite field elements using mpz_class type values.
    explicit Z2(mpz_class const&);

    /// @brief Constructs finite field elements using strings and bases(default decimal).
    explicit Z2(std::string const&, int base = 10);

    /// @brief Converts a finite field element to a double type representation.
    template <typename Int> requires (std::integral<Int> && sizeof(Int) <= sizeof(mp_limb_t))
    explicit operator Int() const;
    explicit operator double() const;

    /// @brief Converts a finite field element to a string representation(default decimal).
    std::string to_string(int base = 10) const;
    /// @brief Converts a finite field element to an mpz_class type representation.
    mpz_class   to_mpz() const;

    /// @brief Assigns a value of integral type to a finite field element. 
    template <std::integral Int>
    void assign(Int);

    /// @brief Assigns a value of floating-point type to a finite field element. 
    template <std::floating_point Float>
    void assign(Float);

    /// @brief Assigns to finite field elements using strings and bases(default decimal).
    void assign(mpz_class const&);
    void assign(std::string const&, int base);

    /// @brief Constructs finite field elements for different number of bits or different sign.
    template <size_t S, bool OtherSigned>
    requires detail::different<K, S> || detail::different_bool<Signed, OtherSigned>
    void assign(Z2<S, OtherSigned> const& other);

    /// @brief Get the value of the bit at the most significant position in the _data.
    bool msb()             const;

    /// @brief Get the value of the bit at the specified position in the _data.
    bool bit(size_t index) const;

    /// @brief Support operation like { Z2<K, S> ans(0); ans.bit(K - 1) = 1; }
    BitReference<limb_t> bit(size_t index);

    /// @brief Get the pointer of underlying data.
    /// @return The pointer of underlying data
    limb_t const*   data() const { return _data;         }
    limb_t*         data()       { return _data;         }

/************************ operators ************************/

    Z2 operator-() const;
    Z2 operator+(const Z2&) const;
    Z2 operator-(const Z2&) const;
    Z2 operator*(const Z2&) const;

    Z2 operator~() const;
    Z2 operator&(const Z2&) const;
    Z2 operator|(const Z2&) const;
    Z2 operator^(const Z2&) const;

    Z2 operator<<(size_type)   const;
    Z2 operator>>(size_type)   const;

    Z2& operator+=(const Z2&);
    Z2& operator-=(const Z2&);
    Z2& operator*=(const Z2&);

    Z2& operator&=(const Z2&);
    Z2& operator|=(const Z2&);
    Z2& operator^=(const Z2&);

    Z2& operator<<=(size_type);
    Z2& operator>>=(size_type);

    bool operator==(const Z2&) const;
    /// @brief Compare this with input.
    /// @return if *this < param, return std::strong_ordering::less
    ///         if *this = param, return std::strong_ordering::equal
    ///         if *this > param, return std::strong_ordering::greater
    std::strong_ordering operator<=>(const Z2&) const;

};

/// @class Z2
/// @brief template specialization for small K
template <size_t K, bool Signed> requires detail::small<K>
class Z2<K, Signed> {
public:
    using size_type = std::size_t;

    /// @brief Unsigned_value_type selects an appropriate unsigned integer type based on template parameter K.v.
    using unsigned_value_type =
        std::conditional_t< K <= 1,   bool,
        std::conditional_t< K <= 8,   uint8_t,
        std::conditional_t< K <= 16,  uint16_t, 
        std::conditional_t< K <= 32,  uint32_t, 
        std::conditional_t< K <= 64,  uint64_t,
        std::conditional_t< K <= 128, uint128_t,
    void>>>>>>;

    using signed_value_type =
        std::conditional_t< K <= 1,   bool,
        std::conditional_t< K <= 8,   int8_t,
        std::conditional_t< K <= 16,  int16_t, 
        std::conditional_t< K <= 32,  int32_t, 
        std::conditional_t< K <= 64,  int64_t,
        std::conditional_t< K <= 128, int128_t,
    void>>>>>>;

    /// @brief Selects value_type based on template parameter Signed.
    using value_type = std::conditional_t<Signed, signed_value_type, unsigned_value_type>;

protected:

    /// @brief Underlying data.
    /// @note Support positive and negative number, negative number 
    ///       are stored in _data as K-bit(not bits of value_type) complement.
    value_type _data;

    /// @brief _mask represents a bit mask value for the unsigned_value_type based on template parameter K.
    static constexpr unsigned_value_type _mask =
        unsigned_value_type(~unsigned_value_type(0)) >> (8*sizeof(unsigned_value_type) - K);

    /// @brief Set extra bits to 0.
    /// @note If K is 8, 16, 32 or 64, then this does not introduce any cost.
    void _normalize();

public:
    static constexpr bool trivially_serializable = true;

    static constexpr Z2 zero() { return Z2(0); }
    static constexpr Z2 one()  { return Z2(1); }
    static constexpr Z2 min()  { return Z2(std::numeric_limits<value_type>::min()); }
    static constexpr Z2 max()  { return Z2(std::numeric_limits<value_type>::max()); }

    /// @brief Return the number of bytes required to represent _data.
    static constexpr size_type size_in_bytes() { return sizeof(_data); }

/************************ ctor, dtor, assign ************************/

    ~Z2()                    = default;
    Z2(Z2&&)                 = default;
    Z2(const Z2&)            = default;
    Z2& operator=(Z2&&)      = default;
    Z2& operator=(const Z2&) = default;

    /// @note If K is 8, 16, 32 or 64, then this does not introduce any cost.
    Z2() { _normalize(); }

    /// @note Allow implicit conversion for convenience.
    Z2(value_type data): _data(data) { _normalize(); }

    /// @brief Explicit conversion for different K.
    template <size_t S, bool otherSigned>
    requires detail::different<K, S> || detail::different_bool<Signed, otherSigned>
    explicit Z2(Z2<S, otherSigned> const& other);

    /// @brief Converts a finite field element to a value_type representation.
    explicit operator value_type() const;

    /// @brief Converts a finite field element to a double type representation.
    template <typename Int> requires std::integral<Int>
    explicit operator Int() const;

    template <std::floating_point T>
    explicit operator T() const;

    /// @brief Converts a finite field element to a string representation(default decimal).
    std::string to_string()         const;

    /// @brief Converts a finite field element to a string representation.
    std::string to_string(int base) const;

    /// @brief Get the value of the bit at the most significant position in the _data.
    bool msb() const;

    /// @brief Get the value of the bit at the specified position in the _data.
    bool bit(size_t index) const;

    /// @brief Support operation like { Z2<K, S> ans(0); ans.bit(K - 1) = 1; }
    BitReference<value_type> bit(size_t index);

    /// @brief Get the pointer of underlying data.
    /// @return The pointer of underlying data
    value_type const*    data() const { return &_data; }
    value_type*          data()       { return &_data; }

/************************ operators ************************/

    Z2  operator-()              const { if constexpr(K == 1) return Z2(~_data);            else return Z2(-_data);            }
    Z2  operator+(const Z2& rhs) const { if constexpr(K == 1) return Z2(_data ^ rhs._data); else return Z2(_data + rhs._data); }
    Z2  operator-(const Z2& rhs) const { if constexpr(K == 1) return Z2(_data ^ rhs._data); else return Z2(_data - rhs._data); }
    Z2  operator*(const Z2& rhs) const { if constexpr(K == 1) return Z2(_data & rhs._data); else return Z2(_data * rhs._data); }

    Z2  operator~()              const { return Z2(~_data);            }
    Z2  operator&(const Z2& rhs) const { return Z2(_data & rhs._data); }
    Z2  operator|(const Z2& rhs) const { return Z2(_data | rhs._data); }
    Z2  operator^(const Z2& rhs) const { return Z2(_data ^ rhs._data); }

    Z2  operator<<(size_t rhs)   const { return Z2(_data << rhs); }
    Z2  operator>>(size_t rhs)   const;

    Z2& operator+=(const Z2& rhs) { if constexpr(K == 1) _data ^= rhs._data;  else { _data += rhs._data;  _normalize(); }  return *this; }
    Z2& operator-=(const Z2& rhs) { if constexpr(K == 1) _data ^= rhs._data;  else { _data -= rhs._data;  _normalize(); }  return *this; }
    Z2& operator*=(const Z2& rhs) { if constexpr(K == 1) _data &= rhs._data;  else { _data *= rhs._data;  _normalize(); }  return *this; }

    Z2& operator&=(const Z2& rhs) { _data &= rhs._data; return *this; }
    Z2& operator|=(const Z2& rhs) { _data |= rhs._data; return *this; }
    Z2& operator^=(const Z2& rhs) { _data ^= rhs._data; return *this; }

    Z2& operator<<=(size_t rhs) { _data <<= rhs; _normalize();  return *this; }
    Z2& operator>>=(size_t rhs);

    bool operator==(const Z2& rhs) const { return _data == rhs._data; }
    /// @brief Compare this with input.
    /// @return if *this < param, return std::strong_ordering::less
    ///         if *this = param, return std::strong_ordering::equal
    ///         if *this > param, return std::strong_ordering::greater
    std::strong_ordering operator<=>(const Z2& rhs) const { return value_type(*this) <=> value_type(rhs); }

};

/// @brief Calculate the multiplicative inverse of a given unsigned binary integer.
template <size_t K>
UnsignedZ2<K> inv(UnsignedZ2<K> const& x);

/// @brief Calculate the square root of a given unsigned binary integer.
template <size_t K>
UnsignedZ2<K> sqrt(UnsignedZ2<K> const& x);

/// @brief Calculate the absolute value of a given binary integer.
template <size_t K, bool S>
Z2<K, S> abs(Z2<K, S> const& x);

/// @brief Count the number of consecutive zeros starting from the significant bit-by-bit.
template <size_t K, bool S>
int countl_zero(Z2<K, S> const&);
