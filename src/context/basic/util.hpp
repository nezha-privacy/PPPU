#pragma once

#include "util.h"

#include "ndarray/ndarray_ref.hpp"
#include "ndarray/tools.hpp"

namespace pppu::detail
{

/// @brief Turn builtin integral type values to integral type values which Value used.
/// @param Fn Function T->U
/// @param T The builtin integral type
/// @param U The integral type
template <typename Fn, std::integral T, typename U = std::invoke_result_t<Fn, T>>
core::NDArrayRef<U> encode(
    Fn&& fn,
    core::NDArrayRef<T> const& input
) {
    core::NDArrayRef<U> output = core::apply(std::forward<Fn>(fn), input);
    return output;
}

/// @brief Turn builtin floating point type values to floating point type values which Value used.
/// @param Fn Function T->U
/// @param T The builtin floating point type
/// @param U The floating point type
template <typename Fn, std::floating_point T, typename U = std::invoke_result_t<Fn, T>>
core::NDArrayRef<U> encode(
    Fn&& fn,
    core::NDArrayRef<T> const& input,
    int64_t fracbits
) {
    core::NDArrayRef<T> scaled_input = core::apply([fracbits](T const& x){
        return std::round( std::ldexp(x, fracbits) );
    }, input);
    core::NDArrayRef<U> output = core::apply(std::forward<Fn>(fn), scaled_input);
    return output;
}

/// @brief Turn integral type values which Value used to builtin integral type values.
/// @param Fn Function U->T
/// @param T The builtin integral type
/// @param U The integral type
template <typename Fn, typename U, typename T = std::invoke_result_t<Fn, U>>
requires std::integral<T>
core::NDArrayRef<T> decode(
    Fn&& fn,
    core::NDArrayRef<U> const& input
) {
    core::NDArrayRef<T> output = core::apply(std::forward<Fn>(fn), input);
    return output;
}

/// @brief Turn floating point type values which Value used to builtin floating point type values.
/// @param Fn Function U->T
/// @param T The builtin floating point type
/// @param U The floating point type
template <typename Fn, typename U, typename T = std::invoke_result_t<Fn, U>>
requires std::floating_point<T>
core::NDArrayRef<T> decode(
    Fn&& fn,
    core::NDArrayRef<U> const& input,
    int64_t fracbits
) {
    core::NDArrayRef<T> scaled_input = core::apply(std::forward<Fn>(fn), input);
    core::NDArrayRef<T> output = core::apply([fracbits](T const& x){
        return std::ldexp(x, -fracbits);
    }, scaled_input);
    return output;
}

} // namespace pppu::detail
