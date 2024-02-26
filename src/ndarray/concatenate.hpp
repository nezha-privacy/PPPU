#pragma once

#include "tools.hpp"

#include "concatenate.h"

namespace core
{

/// @brief Concatenate multiple NDArrayRef arrays.
/// @param arrays std::span of the NDArrayRef array to be connected
/// @param axis Axis of the connection(default 0)
/// @return The result NDArrayRef
template <typename dtype>
NDArrayRef<dtype> concatenate(
    std::span<NDArrayRef<dtype>> arrays,
    int64_t axis
) {
    std::vector<std::span<const int64_t>> old_shapes(arrays.size());
    for(int64_t i = 0; i < arrays.size(); ++i) {
        old_shapes[i] = std::span<const int64_t>(arrays[i].shape());
    }

    auto new_shape = detail::deduceConcatenateShape(old_shapes, axis);

    int64_t new_offset  = 0;
    auto    new_numel   = detail::calcNumel(new_shape);
    auto    new_strides = detail::makeCompactStrides(new_shape);
    auto    new_buffer  = std::make_shared< typename NDArrayRef<dtype>::BufferType >( new_numel );

    int64_t ndim = new_shape.size();
    if(axis < 0) axis += ndim;

    {
    dtype* new_data = new_buffer->data();
    int64_t offset = 0;
    // index is used to track the index value of the current iteration.
    std::vector<int64_t> index(axis, 0);
    // shape is used to determine the termination condition of the iteration.
    std::span<const int64_t> shape(new_shape.data(), axis);

    /// @note For two arrays concatenate, when array1.shape = {2,2}; array2.shape = {2,2}, 
    ///       when axis = 1, the connection sequence is array1[:][0] + array2[:][0] + array1[:][1] + array2[:][1], 
    ///       when axis = 0, the connection sequence is array1[:][:] + array2[:][:].

    do {
        for(auto& arr: arrays) {
            auto subarr = arr.slice(index);
            for_each(subarr, [new_data, offset](int64_t i, dtype const& x){
                new_data[offset + i] = x;
            });
            offset += subarr.numel();
        }
    }
    while(
        -1 != detail::indexIncerment(index, shape)
    );
    }

    return { std::move(new_buffer), std::move(new_shape), std::move(new_strides), new_offset };
}

} // namespace core
