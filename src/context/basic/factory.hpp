#pragma once

#include "factory.h"

#include "util.hpp"
#include "serialization/serialization.hpp"

namespace pppu
{

/// @brief Create a public Value object based on a plain Value.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @param fracbits Fixed point number decimal point position
/// @return The public Value object
template <typename Value>
Value make_public(Context* ctx, typename Value::PlainType data, int64_t fracbits)
{
    Value ans;
    ans.assign_p( std::move(data) );
    ans.set_fracbits(fracbits);
    ans.set_visibility(Visibility::Public());
    return ans;
}

/// @brief Create a private Value object based on a plain Value.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @param fracbits Fixed point number decimal point position
/// @return The private Value object
template <typename Value>
Value make_private(Context* ctx, typename Value::PlainType data, int64_t fracbits)
{
    // broadcast shape and fracbits
    Serializer sr; sr << data.shape() << fracbits;
    ctx->netio()->broadcast(sr.finalize());

    // set value
    Value ans;
    ans.assign_p( std::move(data) );
    ans.set_fracbits(fracbits);
    ans.set_visibility(Visibility::Private(ctx->pid()));
    return ans;
}

/// @brief Create a private empty Value object for a specific player.
/// @param ctx The calculation settings
/// @param owner The player who owns the private Value
/// @return The private Value object
template <typename Value>
Value make_private(Context* ctx, playerid_t owner)
{
    if(owner >= ctx->num_parties())
        throw std::invalid_argument("invalid owner id");

    if(owner == ctx->pid())
        throw std::invalid_argument("use the other overload version for local private input");

    // recv shape and fracbits from owner
    std::vector<int64_t> shape;
    int64_t fracbits;
    ByteVector msg = ctx->netio()->recv(owner);
    Deserializer dr(std::move(msg));
    dr >> shape >> fracbits;

    // use fake data here, does not consume memory, only records shape of the array
    Value ans;
    using pdtype = Value::PlainType::value_type;
    auto fake_data = core::make_ndarray(pdtype{}, std::move(shape), false);
    ans.assign_p(fake_data);
    ans.set_fracbits(fracbits);
    ans.set_visibility(Visibility::Private(owner));
    return ans;
}

/// @brief Create a constant public Value object based on a integral value.
/// @param ctx The calculation settings
/// @param shape The shape of this constant Value
/// @param expand Whether expand this constant Value to this shape
/// @return The constant Value object
template <typename Value, std::integral T>
Value make_constant(Context* ctx, T data, std::vector<int64_t> shape, bool expand)
{
    core::NDArrayRef<T> arr = core::make_ndarray(data, std::move(shape), expand);
    return make_public<Value>(ctx, arr);
}

/// @brief Create a constant public Value object based on a floating point value.
/// @param ctx The calculation settings
/// @param shape The shape of this constant Value
/// @param expand Whether expand this constant Value to this shape
/// @return The constant Value object
template <typename Value, std::floating_point T>
Value make_constant(Context* ctx, T data, std::vector<int64_t> shape, int64_t fracbits, bool expand)
{
    core::NDArrayRef<T> arr = core::make_ndarray(data, std::move(shape), expand);
    return make_public<Value>(ctx, arr, fracbits);
}

/// @brief Create a public Value object based on an integral NDArray.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @return The public Value object
template <typename Value, std::integral T>
Value make_public(Context* ctx, core::NDArrayRef<T> const& data)
{
    // TODO support user defined function
    using U = Value::PlainType::value_type;
    static_assert( std::constructible_from<U, T> );
    auto fn = [](T const x){ return U(x); };

    core::NDArrayRef<U> encoded_data = detail::encode(std::forward<decltype(fn)>(fn), data);
    return make_public<Value>(ctx, std::move(encoded_data), 0);
}

/// @brief Create a public Value object based on a floating point NDArray.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @param fracbits Fixed point number decimal point position
/// @return The public Value object
template <typename Value, std::floating_point T>
Value make_public(Context* ctx, core::NDArrayRef<T> const& data, int64_t fracbits)
{
    // TODO support user defined function
    using U = Value::PlainType::value_type;
    static_assert( std::constructible_from<U, T> );
    auto fn = [](T const x){ return U(x); };
    
    if(fracbits == -1)
        fracbits = ctx->fxp_fracbits();

    core::NDArrayRef<U> encoded_data = detail::encode(std::forward<decltype(fn)>(fn), data, fracbits);
    return make_public<Value>(ctx, std::move(encoded_data), fracbits);
}

/// @brief Create a private Value object based on an integral NDArray.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @return The private Value object
template <typename Value, std::integral T>
Value make_private(Context* ctx, core::NDArrayRef<T> const& data)
{
    // TODO support user defined function
    using U = Value::PlainType::value_type;
    static_assert( std::constructible_from<U, T> );
    auto fn = [](T const x){ return U(x); };

    core::NDArrayRef<U> encoded_data = detail::encode(std::forward<decltype(fn)>(fn), data);
    return make_private<Value>(ctx, std::move(encoded_data), 0);
}

/// @brief Create a private Value object based on a floating point NDArray.
/// @param ctx The calculation settings
/// @param data The input plain Value object
/// @param fracbits Fixed point number decimal point position
/// @return The private Value object
template <typename Value, std::floating_point T>
Value make_private(Context* ctx, core::NDArrayRef<T> const& data, int64_t fracbits)
{
    // TODO support user defined function
    using U = Value::PlainType::value_type;
    static_assert( std::constructible_from<U, T> );
    auto fn = [](T const x){ return U(x); };
    
    if(fracbits == -1)
        fracbits = ctx->fxp_fracbits();

    core::NDArrayRef<U> encoded_data = detail::encode(std::forward<decltype(fn)>(fn), data, fracbits);
    return make_private<Value>(ctx, std::move(encoded_data), fracbits);
}

} // namespace pppu
