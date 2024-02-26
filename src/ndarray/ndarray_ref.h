#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <span>
#include <memory>
#include <functional>

#include "buffer.hpp"
#include "iterator.hpp"
#include "slice.hpp"
#include "util.hpp"

namespace core
{
/// @tparam Type of the element in the dtype array.
/// @details NDArrayRef class provides an interface for accessing and manipulating arrays, 
///          including data Pointers, number of elements, step size, offset, and so on.
///          It also defines iterator types and associated operator overloads 
///          for easy traversal and access to elements in an array.
///          In addition, it also provides some auxiliary methods, 
///          such as type conversion, slicing operations, etc.
template <typename dtype>
class NDArrayRef
{ 
  /// @note  Expand is used when all values in dimension i are equal to their corresponding first value,
  ///        use multiple storage Spaces when expand = true,
  ///        use one storage Spaces when expand = true,and set stride = 0.

  public:
    using BufferType = Buffer<dtype>;

  protected:
    /// @brief _data is a pointer to the buffer where the data is stored.
    std::shared_ptr<BufferType> _buffer;
    /// @brief Store array shape information.
    std::vector<int64_t> _shape;
    /// @brief _stride indicates the difference in buffer between two adjacent elements on the same axis.
    std::vector<int64_t> _strides;
    /// @brief _offset indicates the offset of the first element in buffer.
    int64_t _offset;

  public:
    using value_type      = dtype;
    using pointer         = BufferType::pointer;
    using reference       = BufferType::reference;
    using const_pointer   = BufferType::const_pointer;
    using const_reference = BufferType::const_reference;
    using iterator        = NDIterator<dtype>;
    using const_iterator  = NDIterator<const dtype>;

    NDArrayRef()                             = delete;
    ~NDArrayRef()                            = default;
    NDArrayRef(NDArrayRef &&)                = default;
    NDArrayRef(NDArrayRef const &)           = default;
    NDArrayRef& operator=(NDArrayRef&&)      = default;
    NDArrayRef& operator=(NDArrayRef const&) = default;

    NDArrayRef(
        std::shared_ptr<BufferType> buffer,
        std::vector<int64_t>        shape,
        std::vector<int64_t>        strides,
        int64_t                     offset)
        : _buffer(std::move(buffer)), _shape(std::move(shape)), _strides(std::move(strides)), _offset(offset)
    {
    }

    /// @brief Return number of dimensions.
    /// @return Number of dimensions
    int64_t ndim() const { return _shape.size(); }

    /// @brief Return number of elements.
    /// @return Number of elements
    int64_t numel() const { return detail::calcNumel(_shape); }

    /// @brief Return shape of NDArray on given dimension.
    /// @param dim Given dimension
    /// @return Shape of NDArray on given dimension
    int64_t shape(int64_t dim) const { return _shape[dim]; }

    /// @brief Return shape of NDArray.
    /// @return Shape of NDArray
    std::vector<int64_t> const& shape() const { return _shape; }

    /// @brief Return strides of NDArray.
    /// @return Strides of NDArray
    std::vector<int64_t> const& strides() const { return _strides; }

    /// @brief Return offset of NDArray.
    /// @return Offset of NDArray
    int64_t offset() const { return _offset; }

    /// @brief Return new NDArrayRef object after slicing.
    /// @param indicies.size() <= shape.size(). accepts both integers and slices
    /// @return NDArrayRef object after slicing
    /// @note Memory allocation: never.
    NDArrayRef slice(std::span<const Slice> indicies) const;
    NDArrayRef slice(std::span<const int64_t> indicies) const;
    NDArrayRef slice(std::vector<std::variant<Slice, int64_t>> const& indicies) const;

    /// @brief Return a new array consisting of elements at specified indicies.
    /// @param indicies.size() <= shape.size(). accepts both integers and slices
    /// @return Array consisting of elements at specified indicies
    /// @note Memory allocation: always, currently only one-dimensional arrays are supported.
    NDArrayRef permute(std::span<const int64_t> indicies) const;

    /// @brief Return a new array, where the elements are substituted by value at specified indices.
    /// @param indicies.size() <= shape.size(). accepts both integers and slices
    /// @param value Input value to substitute
    /// @return Array, where the elements are substituted by value at specified indices
    /// @note Memory allocation: always, currently only one-dimensional arrays are supported.
    NDArrayRef substitute(std::span<const int64_t> indicies, NDArrayRef value) const;

    /// @brief Return a reference to the element at the specified index location.
    /// @param index.size() == shape.size()
    /// @return Reference to the element at the specified index location
    reference       elem(std::vector<int64_t> const &index);
    const_reference elem(std::vector<int64_t> const &index) const;

    /// @brief Readjust the NDArrayRef shape to the given new shape.
    /// @param new_shape New shape of value
    /// @return Array with given shape
    /// @note Memory allocation: when this array does not have linear strides.
    NDArrayRef reshape(std::vector<int64_t> new_shape) const;

    /// @brief Readjust the NDArrayRef shape to the given new shape.
    /// @param new_shape New shape of value
    /// @return Array with given shape
    /// @note Memory allocation: never.
    NDArrayRef broadcast_to(std::vector<int64_t> new_shape) const;

    /// @brief Return a new array after the order of the swapped dimensions.
    /// @return New array after the order of the swapped dimensions
    /// @note Memory allocation: never.
    NDArrayRef transpose() const;

    /// @brief Return a compact array with the same content.
    /// @return Compact array with the same content
    /// @note Memory allocation: when this is not a compact array.
    NDArrayRef compact() const;

    /// @brief Return a compact copy of original data.
    /// @return Compact copy of original data
    /// @note Memory allocation: always.
    NDArrayRef copy() const;

    /// @brief Return a new array with elements of new_typeM
    /// @return New array with elements of new_typeM
    /// @note Memory allocation: alwaysM
    template <typename new_type>
    requires (std::constructible_from<new_type, dtype>)
    NDArrayRef<new_type> as() const;

    /// @brief Return a pointer to buffer.
    /// @return Pointer to buffer
    std::shared_ptr<BufferType> sptr() const { return _buffer; }

    /// @brief Return a pointer to a contiguous memory region of elements.
    /// @return pointer to a contiguous memory region of elements
    pointer       data()       { return _buffer->data(); }
    const_pointer data() const { return _buffer->data(); }

    /// @brief Return a string representation of the ArrayRef.
    /// @return String representation of the ArrayRef
    /// @example ArrayRef<int> arr : {1, 2; 3, 4}, 
    ///          arr.to_string : "1 2
    ///                           3 4".
    std::string to_string() const;

    /// @brief General iterator to const element, always valid.
    const_iterator begin() const;
    const_iterator end()   const;

    /// @brief General iterator, always valid.
    iterator begin();
    iterator end();

    /// @brief Linear iterator to const element, faster, only valid for array with linear/compact strides.
    LNIterator<const dtype> lbegin() const;
    LNIterator<const dtype> lend()   const;

    /// @brief Linear iterator, faster, only valid for array with linear/compact strides.
    LNIterator<dtype> lbegin();
    LNIterator<dtype> lend();
};

/// @brief Make an empty NDArrayRef.
/// @param shape Store new array shape information
/// @return New NDArrayRef
template <typename dtype>
NDArrayRef<dtype> make_ndarray (std::vector<int64_t> shape);

/// @brief Make a NDArrayRef filled with value.
/// @param shape Store new array shape information
/// @param value Used to initialize the elements of the NDArrayRef
/// @param expand Indicate whether to expand the NDArrayRef by repeating the value(default: false)
/// @return New ArrayRef
template <typename dtype>
NDArrayRef<dtype> make_ndarray (dtype const& value, std::vector<int64_t> shape, bool expand=false);

/// @brief Create a NDArrayRef object from a std::vector<dtype>.
/// @param data Containing elements to be assigned to the NDArrayRef
/// @return New NDArrayRef
template <typename dtype>
NDArrayRef<dtype> make_ndarray (std::vector<dtype> data);

/// @brief Create a NDArrayRef object from a std::initializer_list<dtype>
/// @param data Containing elements to be assigned to the NDArrayRef
/// @return New NDArrayRef
template <typename dtype>
NDArrayRef<dtype> make_ndarray (std::initializer_list<dtype> data);

/// @brief Generate a NDArrayRef object that contains a sequence of values within the specified range. 
/// @param start Start value of the sequence
/// @param stop End value of the sequence (not included in the sequence)
/// @param step Length between adjacent elements in the step sequence (default 1)
/// @return Contains the NDArrayRef object for a sequence of values within the specified range
template <typename dtype>
NDArrayRef<dtype> arange(dtype start, dtype stop, dtype step=1);

/// @brief Make a NDArrayRef filled with 0.
/// @param shape Store new array shape information
/// @param expand Indicate whether to expand the NDArrayRef by repeating the value(default: false)
/// @return New ArrayRef
template <typename dtype>
NDArrayRef<dtype> zeros (std::vector<int64_t> shape, bool expand=false);

/// @brief Make a NDArrayRef filled with 1.
/// @param shape Store new array shape information
/// @param expand Indicate whether to expand the NDArrayRef by repeating the value(default: false)
/// @return New ArrayRef
template <typename dtype>
NDArrayRef<dtype> ones  (std::vector<int64_t> shape, bool expand=false);

} // namspace core
