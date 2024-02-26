#pragma once

#include "ndarray_ref.hpp"

#include <optional>

namespace core
{
///@brief performs packbits for the input NDArrayRef
///@param in NDArray to perform bit-packing operation.
///@param _axis Specifies the axis of the packaging operation, default no-value, which mean output is one-dimensional
///@return The packed NDArray, where each element represents the bit-packed result at the corresponding position in the input
NDArrayRef<uint8_t> packbits(NDArrayRef<uint8_t> const& in, std::optional<int64_t> _axis = std::optional<int64_t>{});

///@brief performs unpackbits for the input NDArrayRef
///@param in NDArray to perform bit-unpacking operation.
///@param _axis Specifies the axis of the packaging operation, default no-value, which mean output is one-dimensional
///@return Unpacked NDArray, where each element represents the bit unpacked result at the corresponding position in the input
NDArrayRef<uint8_t> unpackbits(NDArrayRef<uint8_t> const& in, std::optional<int64_t> _axis = std::optional<int64_t>{});

} // namspace core
