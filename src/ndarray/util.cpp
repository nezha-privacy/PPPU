#include "util.hpp"

#include <cassert>
#include <stdexcept>

#include <fmt/format.h>

namespace core
{

namespace detail
{

/// @brief Calculate number of elements of given shape.
/// @return Number of elements of given shape
/// @note Multiplication of all elements of shape.
int64_t calcNumel(std::span<const int64_t> shape)
{
    return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>{});
}

/// @brief Determines whether the given strides represent a compact (contiguous) memory layout.
/// @return If is compact strides, return true
/// @note Check if shape[ndim-1] == 1 && isLinearStrides(strides, shape).
bool isCompactStrides(
    std::span<const int64_t> strides,
    std::span<const int64_t> shape)
{
    assert( strides.size() == shape.size() );

    auto compact_strides = makeCompactStrides(shape);
    for(size_t i = 0; i < strides.size(); ++i) {
        if( compact_strides[i] != strides[i] )
            return false;
    }
    return true;
}

/// @brief Generates a compact array based on a given shape.
/// @note makeLinearStrides(1,shape).
/// @return std::vector<int64_t> array represents a compace stride
std::vector<int64_t> makeCompactStrides(
    std::span<const int64_t> shape)
{
    return makeLinearStrides(1, shape);
}

/// @brief Determines whether the given strides represent a compact (contiguous) memory layout.
/// @return If is linear strides, return true
/// @note Check if stride[i] / stride[0] == shape[i+1] * ... * shape[ndim-1].
bool isLinearStrides(
    std::span<const int64_t> strides,
    std::span<const int64_t> shape)
{
    if (shape.size() <= 1)
        return true;

    int64_t ndim = shape.size();
    int64_t stride = strides[ndim - 1] * shape[ndim - 1];
    for (int64_t i = ndim - 2; i >= 0; --i) {
        if (strides[i] != stride)
            return false;
        stride *= shape[i];
    }
    return true;
}

/// @brief Determine whether two shapes can be broadcast(like numpy).
/// @return Broadcast shape if compatible
std::optional<std::vector<int64_t>> broadcastCompatible(
    std::span<const int64_t> lhs,
    std::span<const int64_t> rhs)
{
    std::vector<int64_t> new_shape;

    if(lhs.size() > rhs.size()) {
        new_shape.insert(new_shape.end(), lhs.begin(), lhs.end());
    } else {
        new_shape.insert(new_shape.end(), rhs.begin(), rhs.end());
    }

    for(std::size_t i = 0; i < std::min(lhs.size(), rhs.size()); i++) {
        auto dim1 = lhs[ lhs.size() - 1 - i ];
        auto dim2 = rhs[ lhs.size() - 1 - i ];
        if( dim1 == dim2 || dim1 == 1 || dim2 == 1 )
            // two dimensions are equal or one of dimensions is 1
            new_shape[ new_shape.size() - 1 - i ] = std::max(dim1, dim2);
        else {
            return {};
        }
    }

    return new_shape;
}

/// @brief Generates a linear step array based on the given step size and shape.
/// @note strides[ndim-1] = step; strides[i] = strides[i+1] * shape[i+1].
/// @return std::vector<int64_t> array represents a linear stride
std::vector<int64_t> makeLinearStrides(
    int64_t step,
    std::span<const int64_t> shape)
{
    int64_t ndim = shape.size();
    if(ndim == 0)    return {};

    std::vector<int64_t> strides(ndim);
    strides[ndim-1] = step;
    for (int64_t i = ndim - 2; i >= 0; --i) {
        strides[i] = strides[i+1] * shape[i+1];
    }

    return strides;
}

/// @brief Change the index to non-negative.
/// @param index Indicates the index in slice
/// @note Check whether index is out of range
/// @return Non-negative index representation
std::vector<int64_t> normalize(
    std::span<const int64_t> index,
    std::span<const int64_t> shape)
{
    assert(index.size() == shape.size());

    std::vector<int64_t> ans(shape.size());
    for (int64_t i = 0; i < shape.size(); ++i) {
        ans[i] = (index[i] < 0) ? (index[i] + shape[i]) : index[i];// non-0 index
        if (ans[i] < 0 || ans[i] >= shape[i]) {
            throw std::out_of_range("");
        }
    }
    return ans;
}

/// @brief Converts the linear_index in the slice to a position in memory.
/// @param linear_index Indicates the linear_index in slice
/// @return The memory location corresponding to linear_index
int64_t calcLinearIndex(
    int64_t linear_index,
    std::span<const int64_t> shape,
    std::span<const int64_t> strides,
    int64_t offset)
{
    int64_t ndim = shape.size();

    int64_t pos = offset;
    for(int64_t i = ndim-1; i >= 0; --i) {
        pos += (linear_index % shape[i]) * strides[i];
        linear_index /= shape[i];
    }
    return pos;
}

/// @brief Converts the ndindex in the slice to a position in memory.
/// @param ndindex Indicates the ndindex in slice
/// @return The memory location corresponding to ndindex
int64_t calcNDIndex(
    std::span<const int64_t> index,
    std::span<const int64_t> strides,
    int64_t offset, int64_t ndim)
{
    assert(index.size() == ndim && strides.size() == ndim);

    // scalar
    if (ndim == 0)
        return offset;

    int64_t pos = offset;
    for (int64_t i = ndim - 1; i >= 0; --i) {
        pos += strides[i] * index[i];
    }
    return pos;
}

/// @return A array increment type of std::vector<int64_t>.
/// @brief increment[i] indicates position change in memory after increment,
///        when the highest change value of the index is i
///        (The i-1 bit carries and the i bit does not carry).
/// @example When ndim = 2, shape = (2,4),
///          increment[1] = stride[1], increment[0] = pos(_data[1][0])- pos(_data[0][3]).
std::vector<int64_t> makeLinearIncrement(
    std::span<const int64_t> strides,
    std::span<const int64_t> shape)
{
    assert(strides.size() == shape.size());

    int64_t ndim = shape.size();
    std::vector<int64_t> increment(ndim);

    int64_t sum = 0;
    for(int64_t i = ndim-1; i >= 0; --i) {
        increment[i] = strides[i] - sum;
        sum += strides[i] * (shape[i]-1);
    }
    return increment;
}

/// @brief Compute the index after incrementing.
/// @param index Changed to the next index
/// @return The position of the highest index changed, -1 if index reaches the end
int64_t indexIncerment(
    std::span<int64_t> index,
    std::span<const int64_t> shape)
{
    assert(index.size() == shape.size());

    // corner case for scalar (array with one single element and empty shape)
    if(index.size() == 0)    return -1;

    int64_t ndim = shape.size();
    int64_t pos = ndim-1;
    int64_t carry = 0;
    do {
        carry      = ((index[pos] + 1) >= shape[pos]);
        index[pos] = ((index[pos] + 1) %  shape[pos]);
        pos -= 1;
    } while( pos >= 0 && carry == 1);

    return pos + 1 - carry;
}

/// @brief Calculate the new shape of the product of two matrices.
/// @return {M,K,N} when {M,K}*{K,N}
std::tuple<int64_t, int64_t, int64_t> deduceMatmulShape(
    std::span<const int64_t> shape1,
    std::span<const int64_t> shape2)
{
    if( ! (shape1.size() == 2) )
        throw std::invalid_argument("input must be a matrix");

    if( ! (shape2.size() == 2) )
        throw std::invalid_argument("input must be a matrix");

    if( ! (shape1[1] == shape2[0]) )
        throw std::invalid_argument("matmul input shape mismatch");

    int64_t M = shape1[0];
    int64_t N = shape2[0];
    int64_t K = shape2[1];

    return { M, N, K };
}

/// @brief Inference the array of shapes after concatenate operations, 
///        based on the given shapes of arrays and the axis of the concatenate.
/// @param shapes The shapes of multiple multidimensional arrays
/// @param axis Specifies the dimension along which the concatenate takes place(support negative)
/// @return New shape after concatenate
/// @note Axis value range (-ndim, ndim-1)
std::vector<int64_t> deduceConcatenateShape(
    std::span<std::span<const int64_t>> shapes,
    int64_t _axis
) {
    int64_t N = shapes.size();
    if(N <= 0) {
        throw std::invalid_argument("concatenate with 0 input");
    }

    int64_t ndim = shapes.front().size();
    int64_t axis = _axis + ndim*(_axis<0);
    if(axis < 0 || axis >= ndim) {
        throw std::invalid_argument(fmt::format(
            "concatenate with invalid axis {}", _axis
        ));
    }

    for(int64_t i = 1; i < N; ++i) {
        if(shapes[i].size() != shapes[0].size()) {
            throw std::invalid_argument("concatenate with incompatible shape");
        }
        for(int64_t j = 0; j < ndim; ++j) {
            if(j != axis && shapes[i][j] != shapes[0][j]) {
                throw std::invalid_argument("concatenate with incompatible shape");
            }
        }
    }

    // calculate new shape
    std::vector<int64_t> new_shape(shapes[0].begin(), shapes[0].end());
    for(int64_t i = 1; i < N; ++i) {
        new_shape[axis] += shapes[i][axis];
    }

    return new_shape;
}

} // namespace detail

} // namespace core
