#pragma once

#include <concepts>

#include "context/context.hpp"
#include "ndarray/ndarray_ref.h"

namespace pppu
{

/************************ local input ************************/

/// @brief Create a public Value object based on an integral NDArray.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @return The public Value object
template <typename Value, std::integral T>
Value make_public(Context* ctx, core::NDArrayRef<T> const& data);

/// @brief Create a public Value object based on a floating point NDArray.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @param fracbits Fixed point number decimal point position
/// @return The public Value object
template <typename Value, std::floating_point T>
Value make_public(Context* ctx, core::NDArrayRef<T> const& data, int64_t fracbits = -1);

/// @brief Create a private Value object based on an integral NDArray.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @return The private Value object
template <typename Value, std::integral T>
Value make_private(Context* ctx, core::NDArrayRef<T> const& data);

/// @brief Create a private Value object based on a floating point NDArray.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @param fracbits Fixed point number decimal point position
/// @return The private Value object
template <typename Value, std::floating_point T>
Value make_private(Context* ctx, core::NDArrayRef<T> const& data, int64_t fracbits = -1);

/// @brief Create a constant public Value object based on a integral value.
/// @param ctx The calculation settings
/// @param shape The shape of this constant Value
/// @param expand Whether expand this constant Value to this shape
/// @return The constant Value object
template <typename Value, std::integral T>
Value make_constant(Context* ctx, T data, std::vector<int64_t> shape, bool expand = false);

/// @brief Create a constant public Value object based on a floating point value.
/// @param ctx The calculation settings
/// @param shape The shape of this constant Value
/// @param expand Whether expand this constant Value to this shape
/// @return The constant Value object
template <typename Value, std::floating_point T>
Value make_constant(Context* ctx, T data, std::vector<int64_t> shape, int64_t fracbits = -1, bool expand = false);

/// @brief Create a public Value object based on a plain Value.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @param fracbits Fixed point number decimal point position
/// @return The public Value object
template <typename Value>
Value make_public(Context* ctx, typename Value::PlainType data, int64_t fracbits);

/// @brief Create a private Value object based on a plain Value.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @param fracbits Fixed point number decimal point position
/// @return The private Value object
template <typename Value>
Value make_private(Context* ctx, typename Value::PlainType data, int64_t fracbits);

/************************ input from other party ************************/

/// @brief Create a private empty Value object for a specific player.
/// @param ctx The calculation settings
/// @param owner The player who owns the private Value
/// @return The private Value object
template <typename Value>
Value make_private(Context* ctx, playerid_t owner);

} // namespace pppu
