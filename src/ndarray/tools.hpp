#pragma once

#include "tools.h"

#include <concepts>
#include <type_traits>

namespace core
{
/************************ flatten & unflatten ************************/

/// @brief Flatten the given NDArrayRef into a one-dimensional array.
/// @param in NDArrayRef to be flattened
/// @return The ArrayRef of the flattened one-dimensional array
/// @note Memory allocation: when this array does not have linear strides.
template <typename dtype>
ArrayRef<dtype> flatten(NDArrayRef<dtype> const& in)
{
    if( detail::isLinearStrides(in.strides(), in.shape()) ) {
        return ArrayRef( std::move( in.sptr()), in.numel(), in.strides().back(), in.offset() );
    } else {
        return flatten(in.copy());
    }
}

/// @brief Unflatten the given one-dimensional array ArrayRef to a NDArrayRef of the specified shape.
/// @param in The ArrayRef of the one-dimensional array to be unflattened
/// @param shape Specifies the shape, passed as std::vector<int64_t>
/// @return The recovered NDArrayRef object with the specified shape
/// @note Memory allocation: never.
template <typename dtype>
NDArrayRef<dtype> unflatten(ArrayRef<dtype> const& in, std::vector<int64_t> shape)
{
    auto new_buffer = in.sptr();
    auto new_strides = detail::makeLinearStrides( in.stride(), shape );
    auto new_offset = in.offset();
    return { std::move(new_buffer), std::move(shape), std::move(new_strides), new_offset };
}

/**************** for each ****************/

/// @brief Traverse all arrays in order.
/// @param fn Accepts the following two types of functions
/// @example 1. void fn ( int64_t linear_index, dtype x ), 
///             the first parameter is the linear index and the second parameter is the corresponding element value.
///          2. void fn ( dtype x ).
template <typename dtype, typename Fn>
void for_each(ArrayRef<dtype> const& in, Fn&& fn)
{   
    // check the parameter types of the function object
    static constexpr bool fn_takes_x          = std::invocable<Fn, dtype>;
    static constexpr bool fn_takes_int_and_x  = std::invocable<Fn, int64_t, dtype>;
    static_assert( fn_takes_x || fn_takes_int_and_x, "invalid fn" );

    auto begin = in.begin();
    auto end   = in.end();
    for(auto iter = begin; iter != end; ++iter)
    {
        if constexpr ( fn_takes_int_and_x ) {
            // call the function object, passing the element index and value as parameters
            std::invoke(std::forward<Fn>(fn), iter.linear_index(), *iter);
        }
        else if constexpr ( fn_takes_x ) {
            // call the function object, passing the value as parameter
            std::invoke(std::forward<Fn>(fn), *iter);
        }
        else {
            throw std::logic_error("impossible");
        }
    }
}

/// @brief Traverse all arrays in order
/// @param fn Accepts the following three types of functions
/// @example 1. void fn ( std::span<const int64_t> nd_index, dtype x ), 
///             the first parameter is the n-dimensional index, and the second parameter is the corresponding element value.
///          2. void fn ( int64_t linear_index, dtype x ), 
///             the first parameter is the linear index and the second parameter is the corresponding element value.
///          3. void fn ( dtype x ).
template <typename dtype, typename Fn>
void for_each(NDArrayRef<dtype> const& in, Fn&& fn)
{
     // check the parameter types of the function object
    static constexpr bool fn_takes_x          = std::invocable<Fn, dtype>;
    static constexpr bool fn_takes_int_and_x  = std::invocable<Fn, int64_t, dtype>;
    static constexpr bool fn_takes_span_and_x = std::invocable<Fn, std::span<const int64_t>, dtype>;
    static_assert( fn_takes_x || fn_takes_int_and_x || fn_takes_span_and_x, "invalid fn" );

    if constexpr ( fn_takes_span_and_x )
    {
        auto begin = in.begin();
        auto end   = in.end();
        // use an index iterator to traverse a multidimensional array and call a function object, 
        // passing the index and element values as parameters
        for(auto iter = begin; iter != end; ++iter)
            std::invoke(std::forward<Fn>(fn), iter.index(), *iter);
    }
    else if constexpr ( fn_takes_x || fn_takes_int_and_x )
    {
        // use linear iterator to speed up compuation
        auto begin = in.lbegin();
        auto end   = in.lend();
        for(auto iter = begin; iter != end; ++iter)
        {
            if constexpr ( fn_takes_int_and_x ) {
                // call the function object, passing the element index and value as parameters
                std::invoke(std::forward<Fn>(fn), iter.linear_index(), *iter);
            }
            else if constexpr ( fn_takes_x ) {
                // call the function object, passing the  value as parameter
                std::invoke(std::forward<Fn>(fn), *iter);
            }
            else {
                throw std::logic_error("impossible");
            }
        }
    }
    else
    {
        throw std::logic_error("impossible");
    }
}

/// @brief Traverse all the elements in the group in order.
/// @param fn Accepts the following three types of functions
/// @example 1. void fn ( std::span<const int64_t> nd_index, NDArrayRef<dtype> x ), 
///             The first argument is an N-1 dimensional index and
///             the second argument is a one-dimensional array of length in.shape[axis].
///          2. void fn ( int64_t linear_index, NDArrayRef<dtype> x ), 
///             The first argument is a linear index and
///             the second argument is a one-dimensional array of length in.shape[axis].
///          3. void fn ( NDArrayRef<dtype> x ).
template <typename dtype, typename Fn>
void for_each_axis(NDArrayRef<dtype> const& in, int64_t axis, Fn&& fn)
{
    static constexpr bool fn_takes_x          = std::invocable<Fn, NDArrayRef<dtype>>;
    static constexpr bool fn_takes_int_and_x  = std::invocable<Fn, int64_t, NDArrayRef<dtype>>;
    static constexpr bool fn_takes_span_and_x = std::invocable<Fn, std::span<const int64_t>, NDArrayRef<dtype>>;
    static_assert( fn_takes_x || fn_takes_int_and_x || fn_takes_span_and_x, "invalid fn" );

    int64_t ndim = in.ndim();
    if( axis < 0 )  axis += ndim;
    //axis has been modified once
    if( axis < 0 || axis >= ndim )
        throw std::invalid_argument(fmt::format("axis {} out of range", axis));

    int64_t     old_offset  = in.offset();
    auto const& old_shape   = in.shape();
    auto const& old_buffer  = in.sptr();
    auto const& old_strides = in.strides();

    // The tmp slice contains the first element along the specified axis, 
    // and the slice range for the other axiss remains the same.
    auto slice_index = std::vector<std::variant<Slice, int64_t>>( ndim, Slice{} );
    slice_index[axis] = int64_t(0);
    auto tmp = in.slice(slice_index);

    auto begin = tmp.begin();
    auto end   = tmp.end();

    /// @example when in = {{1, 2, 3}, 
    ///                     {4, 5, 6}, 
    ///                     {7, 8, 9}} ; axis = 1;
    ///          tmp = {1, 2, 3};
    ///          the for loop loops three times, each time with new_array {1,4,7}, {2,5,8}, {3,6,9}
    for(auto iter = begin; iter != end; ++iter)
    {
        int64_t new_offset  = iter.pos();
        auto    new_buffer  = old_buffer;
        auto    new_shape   = std::vector<int64_t>{ old_shape[axis] };
        auto    new_strides = std::vector<int64_t>{ old_strides[axis] };

        NDArrayRef<dtype> new_array{ std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };

        if constexpr ( fn_takes_span_and_x ) {
            std::invoke(std::forward<Fn>(fn), iter.index(), new_array);
        }
        else if constexpr ( fn_takes_int_and_x ) {
            std::invoke(std::forward<Fn>(fn), iter.linear_index(), new_array);
        }
        else if constexpr ( fn_takes_x ) {
            std::invoke(std::forward<Fn>(fn), new_array);
        }
        else {
            throw std::logic_error("impossible");
        }
    }
}

/************************ reduce ************************/

/// @brief A dimensionality reduction operation is performed on the 'in' using 'fn'.
/// @param fn (dtype, dtype) -> dtype
/// @param in The NDArrayRef object of the multidimensional array to be reduced
/// @param axis If none, return one single element
/// @param initial_value Used to add the result of dimension reduction.
/// @param keep_dims If true, the output array will have the same dimension as the input
template <typename Fn, typename dtype>
requires (std::same_as<std::invoke_result_t<Fn, dtype, dtype>, dtype>)
NDArrayRef<dtype> reduce(
    Fn&& fn,
    NDArrayRef<dtype> const& in,
    std::optional<int64_t> _axis,
    dtype initial_value,
    bool keep_dims)
{
    if( _axis.has_value() )
    {
        int64_t ndim = in.ndim();
        int64_t axis = _axis.value();
        if(axis < 0)    axis += ndim;

        if( axis < 0 || axis >= ndim )
            throw std::invalid_argument(fmt::format("invalid axis"));

        if( in.shape(axis) <= 0 )
            throw std::invalid_argument("invalid axis");

        auto const& old_data    = in.data();
        auto const& old_shape   = in.shape();
        auto const& old_offset  = in.offset();
        auto const& old_strides = in.strides();

        int64_t new_offset = 0;
        auto    new_shape  = old_shape;  new_shape[axis] = 1;
        int64_t new_numel  = detail::calcNumel( new_shape );
        auto    new_buffer = std::make_shared<typename NDArrayRef<dtype>::BufferType>( new_numel );
        auto    new_data   = new_buffer->data();

        // fn = std::tuple<Fn>(std::forward<Fn>(fn)) is used to capture the reduction function fn 
        // instead of the specific operation represented by fn.
        for_each_axis(in, axis, [&initial_value, new_data, fn=std::tuple<Fn>(std::forward<Fn>(fn))](int64_t i, NDArrayRef<dtype> arr){
            dtype sum = initial_value;
            // execute 'fn' for each element along the axis
            for_each(arr, [ &sum, &fn ](dtype x){
                sum = std::invoke(std::get<0>(fn), sum, x);
            });
            new_data[i] = sum;
        });

        if(keep_dims == false)
            new_shape.erase( new_shape.begin() + axis );
        auto new_strides = detail::makeCompactStrides( new_shape );

        return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
    }
    else
    {
        dtype sum = initial_value;
        for_each(in, [&sum, fn=std::tuple<Fn>(std::forward<Fn>(fn))](dtype const& x){
            sum = std::invoke(std::get<0>(fn), sum, x);
        });
        auto    new_buffer  = std::make_shared< typename NDArrayRef<dtype>::BufferType >(1, sum);
        auto    new_shape   = std::vector<int64_t> {};
        auto    new_strides = std::vector<int64_t> {};
        int64_t new_offset  = 0;
        return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
    }
}

/************************ element wise op ************************/

/// @brief Element wise operation.
/// @param fn(dtype) -> dtype
/// @param in Input value to be applied
/// @return Applied value
template <typename Fn, typename dtype, typename rtype>
ArrayRef<rtype> apply(Fn&& fn, ArrayRef<dtype> const& in)
{
    // static_assert( std::same_as<rtype, void> == false );
    static_assert(  std::same_as< std::invoke_result_t<Fn, dtype>, void> == false );
    if(in.stride() == 0) {
        auto new_buffer = std::make_shared< typename ArrayRef<rtype>::BufferType >( 1 );
        new_buffer->data()[0] = std::invoke(std::forward<Fn>(fn), in[0]);
        return { std::move(new_buffer), in.numel(), 0, 0 };
    }

    int64_t numel = in.numel();
    int64_t new_stride = 1;
    int64_t new_offset = 0;
    auto new_buffer = std::make_shared< typename ArrayRef<rtype>::BufferType >(numel);
    auto new_data = new_buffer->data();

    auto iter = in.begin();
    for(int64_t i = 0; i < numel; ++i, ++iter) {
        new_data[i] = std::invoke(std::forward<Fn>(fn), *iter);
    }

    return { std::move(new_buffer), numel, new_stride, new_offset };
}

/// @brief Element wise operation.
/// @param fn(dtype1, dtype2) -> rtype
/// @param lhs First input value to be applied
/// @param lhs Second input value to be applied
/// @return Applied value
template <typename Fn, typename dtype1, typename dtype2, typename rtype>
ArrayRef<rtype> apply(Fn&& fn, ArrayRef<dtype1> const& lhs, ArrayRef<dtype2> const& rhs) {
    // static_assert( std::same_as<rtype, void> == false );
    static_assert(  std::same_as< std::invoke_result_t<Fn, dtype1, dtype2>, void> == false );
    if( lhs.numel() != rhs.numel() ) {
        throw std::invalid_argument("number of elements mismatch");
    }

    if(lhs.stride() == 0)
    {
        return apply(
            [fn = std::tuple<Fn>( std::forward<Fn>(fn) ),
            x = lhs[0] ](auto const& y){ return std::invoke(std::get<0>(fn), x, y); },
            rhs
        );
    }
    else if(rhs.stride() == 0)
    {
        return apply(
            [fn = std::tuple<Fn>( std::forward<Fn>(fn) ),
            y = rhs[0] ](auto const& x){ return std::invoke(std::get<0>(fn), x, y); },
            lhs
        );
    }

    int64_t numel = lhs.numel();
    int64_t new_stride = 1;
    int64_t new_offset = 0;
    auto    new_buffer = std::make_shared< typename ArrayRef<rtype>::BufferType >(numel);
    auto    new_data   = new_buffer->data();

    auto liter = lhs.begin();
    auto riter = rhs.begin();
    for(int64_t i = 0; i < numel; ++i, ++liter, ++riter) {
        new_data[i] = std::invoke(std::forward<Fn>(fn), *liter, *riter);
    }

    return { std::move(new_buffer), numel, new_stride, new_offset };
}

/// @brief Element wise operation.
/// @param fn(dtype) -> dtype
/// @param in Input value to be applied
/// @return Applied value
template <typename Fn, typename dtype, typename rtype>
NDArrayRef<rtype> apply(Fn&& fn, NDArrayRef<dtype> const& in)
{
    int64_t     old_numel = in.numel();
    auto const& old_shape = in.shape();

    int64_t new_offset  = 0;
    int64_t new_numel   = old_numel;
    auto    new_shape   = old_shape;
    auto    new_strides = detail::makeCompactStrides(new_shape);
    auto    new_buffer  = std::make_shared< typename NDArrayRef<rtype>::BufferType >( new_numel );
    auto    new_data    = new_buffer->data();

    for_each(in, [new_data, fn=std::tuple<Fn>(std::forward<Fn>(fn))](int64_t i, dtype const& x){
        new_data[i] = std::invoke(std::get<0>(fn), x);
    });

    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

/// @brief Element wise operation.
/// @param fn(dtype1, dtype2) -> rtype
/// @param lhs First input value to be applied
/// @param lhs Second input value to be applied
/// @return Applied value
template <typename Fn, typename dtype1, typename dtype2, typename rtype>
NDArrayRef<rtype> apply(Fn&& fn, NDArrayRef<dtype1> const& lhs, NDArrayRef<dtype2> const& rhs)
{
    if( lhs.shape() != rhs.shape() ) {
        throw std::invalid_argument("shape of arrays mismatch");
    }

    int64_t new_offset  = 0;
    int64_t new_numel   = lhs.numel();
    auto    new_shape   = lhs.shape();
    auto    new_strides = detail::makeCompactStrides(new_shape);
    auto    new_buffer  = std::make_shared< typename NDArrayRef<rtype>::BufferType >( new_numel );
    auto    new_data    = new_buffer->data();

    auto foo = [new_data, new_numel, fn=std::tuple<Fn>(std::forward<Fn>(fn))](auto lhs, auto rhs){
        for(int64_t i = 0; i < new_numel; ++i, ++lhs, ++rhs) {
            new_data[i] = std::invoke(std::get<0>(fn), *lhs, *rhs);
        }
    };

    bool lhs_is_linear = detail::isLinearStrides(lhs.strides(), lhs.shape());
    bool rhs_is_linear = detail::isLinearStrides(rhs.strides(), rhs.shape());

    if( lhs_is_linear && rhs_is_linear ) {
        foo(lhs.lbegin(), rhs.lbegin());
    }
    else if ( lhs_is_linear && !rhs_is_linear ) {
        foo(lhs.lbegin(), rhs.begin());
    }
    else if ( !lhs_is_linear && rhs_is_linear ) {
        foo(lhs.begin(), rhs.lbegin());
    }
    else {
        foo(lhs.begin(), rhs.begin());
    }

    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

} // namespace core
