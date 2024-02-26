#pragma once


#include "serialization/serialization.hpp"

#include "concepts.hpp"

#include "array_ref.hpp"
#include "ndarray_ref.hpp"

/// @brief Serializer for ArrayRef.
/// @param sr Serializer reference
/// @param array ArrayRef const reference
template <typename dtype>
void serialize(Serializer& sr, core::ArrayRef<dtype> const& array)
{
    sr << array.numel();
    core::for_each(array, [&sr](auto const& x){
        sr << x;
    });
}

/// @brief Deserializer for ArrayRef.
/// @param sr Deserializer reference
/// @return Output ArrayRef
template <isArrayRef ArrayRefType>
ArrayRefType deserialize_get(Deserializer& dr)
{
    int64_t numel;
    int64_t stride = 1;
    int64_t offset = 0;

    dr >> numel;

    auto buffer = std::make_shared< typename ArrayRefType::BufferType >(numel);
    auto data = buffer->data();

    dr >> std::span( data, static_cast<std::size_t>(numel) );

    return { std::move(buffer), numel, stride, offset };
}

/// @brief Serializer for NDArrayRef.
/// @param sr Serializer reference
/// @param array NDArrayRef const reference
template <typename dtype>
void serialize(Serializer& sr, core::NDArrayRef<dtype> const& array)
{
    sr << array.shape();
    core::for_each(array, [&sr](auto const& x){
        sr << x;
    });
}

/// @brief Deserializer for NDArrayRef.
/// @param sr Deserializer reference
/// @return Output NDArrayRef
template <isNDArrayRef NDArrayRefType>
NDArrayRefType deserialize_get(Deserializer& dr)
{
    auto    shape   = dr.get< std::vector<int64_t> >();
    auto    strides = core::detail::makeCompactStrides(shape);
    int64_t numel   = core::detail::calcNumel(shape);
    int64_t offset  = 0;

    auto buffer = std::make_shared< typename NDArrayRefType::BufferType >( numel );
    auto data   = buffer->data();

    dr >> std::span( data, static_cast<std::size_t>(numel) );

    return { std::move(buffer), std::move(shape), std::move(strides), offset };
}
