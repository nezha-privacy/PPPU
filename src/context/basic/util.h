#pragma once

#include "ndarray/ndarray_ref.h"

namespace pppu::detail
{

/// @brief Turn builtin integral type values to integral type values which Value used.
/// @param Fn Function T->U
/// @param T The builtin integral type
/// @param U The integral type
template <typename Fn, std::integral T, typename U>
core::NDArrayRef<U> encode(
    Fn&& fn,
    core::NDArrayRef<T> const& input
);

/// @brief Turn integral type values which Value used to builtin integral type values.
/// @param Fn Function U->T
/// @param T The builtin integral type
/// @param U The integral type
template <typename Fn, typename U, typename T>
requires std::integral<T>
core::NDArrayRef<T> decode(
    Fn&& fn,
    core::NDArrayRef<U> const& input
);

/// @brief Turn builtin floating point type values to floating point type values which Value used.
/// @param Fn Function T->U
/// @param T The builtin floating point type
/// @param U The floating point type
template <typename Fn, typename T, typename U>
core::NDArrayRef<U> encode(
    Fn&& fn,
    core::NDArrayRef<T> const& input,
    int64_t fracbits
);

/// @brief Turn floating point type values which Value used to builtin floating point type values.
/// @param Fn Function U->T
/// @param T The builtin floating point type
/// @param U The floating point type
template <typename Fn, typename U, typename T>
requires std::floating_point<T>
core::NDArrayRef<T> decode(
    Fn&& fn,
    core::NDArrayRef<U> const& input,
    int64_t fracbits
);

} // namespace pppu::detail
