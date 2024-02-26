#pragma once

#include "ndarray_ref.hpp"

namespace core
{
/// @brief Concatenate multiple NDArrayRef arrays.
/// @param arrays std::span of the NDArrayRef array to be connected
/// @param axis Axis of the connection(default 0)
/// @return The result NDArrayRef
template <typename dtype>
NDArrayRef<dtype> concatenate(
    std::span<NDArrayRef<dtype>> arrays,
    int64_t axis = 0
);

} // namespace core
