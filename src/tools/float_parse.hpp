#pragma once

#include <cstdint>
#include <numeric>
#include <concepts>
#include <tuple>

/// @struct FloatParseInfo
/// @brief Bitwise processing of double type.
template <typename T>
struct FloatParseInfo;

/// @struct FloatParseInfo
/// @brief Bitwise processing of float type.
template <>
struct FloatParseInfo<float>
{
    static_assert(sizeof(float) == 4);

    using float_type = float;
    using int_type = int32_t;
    using size_type = std::size_t;

    static constexpr size_type sign_offset = 31;
    static constexpr size_type expo_offset = 23;
    static constexpr size_type sigd_bits   = 23;

    static constexpr int_type bias = 127;
    static constexpr int_type unit_mask = 0x00800000;
    static constexpr int_type sigd_mask = 0x007fffff;
    static constexpr int_type expo_mask = 0x000000ff;
};

/// @struct FloatParseInfo
/// @brief Bitwise processing of double type.
template <>
struct FloatParseInfo<double>
{   
    static_assert(sizeof(double) == 8);

    using float_type = double;
    using int_type = int64_t;
    using size_type = std::size_t;

    static constexpr size_type sign_offset = 63;
    static constexpr size_type expo_offset = 52;
    static constexpr size_type sigd_bits   = 52;

    static constexpr int_type bias = 1023;
    static constexpr int_type unit_mask = 0x0010000000000000;
    static constexpr int_type sigd_mask = 0x000fffffffffffff;
    static constexpr int_type expo_mask = 0x00000000000007ff;

};

/// @struct FloatParse
/// @brief Bitwise processing of float type.
template <typename float_type>
struct FloatParse
{   
    using int_type = typename FloatParseInfo<float_type>::int_type;

    /// @brief The number of significand bits.
    static constexpr auto sigd_bits   = FloatParseInfo<float_type>::sigd_bits;
    /// @brief The offset of the sign bits.
    static constexpr auto sign_offset = FloatParseInfo<float_type>::sign_offset;
    /// @brief The offset of the exponential bits.
    static constexpr auto expo_offset = FloatParseInfo<float_type>::expo_offset;

    /// @brief The offset value is used to calculate the offset of the exponent.
    static constexpr auto bias        = FloatParseInfo<float_type>::bias;
    /// @brief Unit mask, used to set the exponent bit to 1.
    static constexpr auto unit_mask   = FloatParseInfo<float_type>::unit_mask;
    /// @brief Significand mask, used to extract significand part.
    static constexpr auto sigd_mask   = FloatParseInfo<float_type>::sigd_mask;
    /// @brief Exponential mask, used to extract the exponential part.
    static constexpr auto expo_mask   = FloatParseInfo<float_type>::expo_mask;

    static int_type int_cast(float_type x)
    {
        return std::bit_cast<int_type>(x);
    }

    /// @brief Get the value of the sign bit.
    static int_type msb(float_type x)
    {
        return (int_cast(x) >> sign_offset) & 1;
    }

    /// @brief Get the value of the significand part stored in int_type.
    static int_type significand(float_type x)
    {
        return int_cast(x) & sigd_mask;
    }

    /// @brief Get the value of the exponential part stored in int_type.
    static int_type exponent(float_type x)
    {
        return (int_cast(x) >> expo_offset) & expo_mask;
    }

    /// @return Integers S, F, E, where x = (-1)^S * F * 2^E
    /// @note When x is Inf or NaN, the result is undefined.
    static std::tuple<int_type, int_type, int_type> parse(float_type x)
    {
        int_type S = msb(x);
        int_type e = exponent(x);
        int_type f = significand(x);

        // if (e = 0), unformatted value; else formatted value
        int_type E = e ? (e - bias)      : (1 - bias);
        int_type F = e ? (f | unit_mask) : f;

        // (E - sigd_bits) is used to handle the decimal point of a fixed point number
        return { S, E - sigd_bits , F };
    }
};
