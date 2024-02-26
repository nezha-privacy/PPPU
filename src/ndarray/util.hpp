#pragma once

#include <span>
#include <vector>
#include <ranges>
#include <cstdint>
#include <numeric>
#include <optional>

namespace core
{

namespace detail
{
/// @note There is no need to reallocate memory when slicing, just change the step size and offset.
///       Offset indicates the offset of the first element in mermory.
///       Strides indicates the difference in memory between two adjacent elements on the same axis,
///       and strides can be negative.

/// @param shape Shape array, containing the size information of the array dimensions.
/// @param strides An array of strides that contains information about the memory layout of elements
/// @param offset Indicates the offset of the first element in mermory
/// @param ndim The length of shape
/// @note Higher dimensions in front, lower dimensions in back.

/// @brief Calculate number of elements of given shape.
/// @return Number of elements of given shape
/// @note Multiplication of all elements of shape.
int64_t calcNumel(
    std::span<const int64_t> shape);

/// @brief Determines whether the given strides represent a compact (contiguous) memory layout.
/// @return If is compact strides, return true
/// @note Check if shape[ndim-1] == 1 && isLinearStrides(strides, shape).
bool isCompactStrides(
    std::span<const int64_t> strides,
    std::span<const int64_t> shape);

/// @brief Determines whether the given strides represent a compact (contiguous) memory layout.
/// @return If is linear strides, return true
/// @note Check if stride[i] / stride[0] == shape[i+1] * ... * shape[ndim-1].
bool isLinearStrides(
    std::span<const int64_t> strides,
    std::span<const int64_t> shape);

/// @brief Generates a compact array based on a given shape.
/// @note makeLinearStrides(1,shape).
/// @return std::vector<int64_t> array represents a compace stride
std::vector<int64_t> makeCompactStrides(
    std::span<const int64_t> shape);

/// @brief Generates a linear step array based on the given step size and shape.
/// @note strides[ndim-1] = step; strides[i] = strides[i+1] * shape[i+1].
/// @return std::vector<int64_t> array represents a linear stride
std::vector<int64_t> makeLinearStrides(
    int64_t step,
    std::span<const int64_t> shape);

/// @brief Determine whether two shapes can be broadcast(like numpy).
/// @return Broadcast shape if compatible
std::optional<std::vector<int64_t>> broadcastCompatible(
    std::span<const int64_t> shape1,
    std::span<const int64_t> shape2);

/// @brief Converts the linear_index in the slice to a position in memory.
/// @param linear_index Indicates the linear_index in slice
/// @return The memory location corresponding to linear_index
int64_t calcLinearIndex(
    int64_t linear_index,
    std::span<const int64_t> shape,
    std::span<const int64_t> strides,
    int64_t offset);

/// @brief Converts the ndindex in the slice to a position in memory.
/// @param ndindex Indicates the ndindex in slice
/// @return The memory location corresponding to ndindex
int64_t calcNDIndex(
    std::span<const int64_t> ndindex,
    std::span<const int64_t> strides,
    int64_t offset, int64_t ndim);

/// @brief Change the index to non-negative.
/// @param index Indicates the index in slice
/// @note Check whether index is out of range
/// @return Non-negative index representation
std::vector<int64_t> normalize(
    std::span<const int64_t> index,
    std::span<const int64_t> shape);

/// @return A array increment type of std::vector<int64_t>.
/// @brief increment[i] indicates position change in memory after increment,
///        when the highest change value of the index is i
///        (The i-1 bit carries and the i bit does not carry).
/// @example When ndim = 2, shape = (2,4),
///          increment[1] = stride[1], increment[0] = pos(_data[1][0])- pos(_data[0][3]).
std::vector<int64_t> makeLinearIncrement(
    std::span<const int64_t> strides,
    std::span<const int64_t> shape);

/// @brief Compute the index after incrementing.
/// @param index Changed to the next index
/// @return The position of the highest index changed, -1 if index reaches the end
int64_t indexIncerment(
    std::span<int64_t> index,
    std::span<const int64_t> shape);

/// @brief Calculate the new shape of the product of two matrices.
/// @return {M,K,N} when {M,K}*{K,N}
std::tuple<int64_t, int64_t, int64_t> deduceMatmulShape(
    std::span<const int64_t> shape1,
    std::span<const int64_t> shape2);

/// @brief Inference the array of shapes after concatenate operations, 
///        based on the given shapes of arrays and the axis of the concatenate.
/// @param shapes The shapes of multiple multidimensional arrays
/// @param axis Specifies the dimension along which the concatenate takes place(support negative)
/// @return New shape after concatenate
std::vector<int64_t> deduceConcatenateShape(
    std::span<std::span<const int64_t>> shapes,
    int64_t axis
);

} // namespace detail

} // namespace core
