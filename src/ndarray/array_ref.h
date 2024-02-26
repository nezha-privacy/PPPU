#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <string>
#include <initializer_list>

#include "slice.hpp"
#include "buffer.hpp"
#include "iterator.hpp"

namespace core
{

/// @tparam Type of the element in the dtype array
/// @details ArrayRef class provides an interface for accessing and manipulating arrays, 
///          including data Pointers, number of elements, step size, offset, and so on.
///          It also defines iterator types and associated operator overloads 
///          for easy traversal and access to elements in an array.
///          In addition, it also provides some auxiliary methods, 
///          such as type conversion, slicing operations, etc.
template <typename dtype>
class ArrayRef
{
  /// @note Expand is used when all values in dimension i are equal to their corresponding first value,
  ///       use multiple storage Spaces when expand = true,
  ///       use one storage Spaces when expand = true,and set stride = 0.

  public:
    using BufferType = Buffer<dtype>;

  protected:

    /// @brief A pointer to the buffer where the data is stored.
    std::shared_ptr<BufferType> _data;
    /// @brief The length of data.
    int64_t _numel;
    /// @brief Indicates the difference in buffer between two adjacent elements.
    int64_t _stride;
    /// @brief Indicates the offset of the first element in buffer.
    int64_t _offset;

  public:

    using value_type      = dtype;
    using pointer         = dtype*;
    using reference       = dtype&;
    using const_pointer   = dtype const*;
    using const_reference = dtype const&;
    using iterator        = LNIterator<dtype>;
    using const_iterator  = LNIterator<dtype const>;

    ArrayRef(std::shared_ptr<BufferType> data, int64_t numel, int64_t stride, int64_t offset)
        : _data( std::move(data) ), _numel(numel), _stride(stride), _offset(offset) {}

    ArrayRef()                           = delete;
    ~ArrayRef()                          = default;
    ArrayRef(ArrayRef&&)                 = default;
    ArrayRef(ArrayRef const&)            = default;
    ArrayRef& operator=(ArrayRef&&)      = default;
    ArrayRef& operator=(ArrayRef const&) = default;

    /// @brief Get the shared pointer _data.
    std::shared_ptr<BufferType> sptr() const { return _data; }

    /// @brief Get the pointer to the data.
    pointer       data()       { return _data->data(); }
    /// @brief Get the pointer to the const data.
    const_pointer data() const { return _data->data(); }

    /// @brief Get the _numel.
    int64_t numel () const { return _numel;  }
    /// @brief Get the _stride.
    int64_t stride() const { return _stride; }
    /// @brief Get the _offset.
    int64_t offset() const { return _offset; }

    /// @brief Get the reference to the element corresponding to the index.
    reference       operator[](int64_t index)       { return this->data()[_offset + _stride * index]; }
    /// @brief Get the reference to the const element corresponding to the index.
    const_reference operator[](int64_t index) const { return this->data()[_offset + _stride * index]; }

    /// @brief Get the start position of the iterator.
    iterator begin();
    /// @brief Get the end position of the iterator.
    iterator end();

    /// @brief Get the start position of the const_iterator.
    const_iterator begin() const;
    /// @brief Get the end position of the const_iterator.
    const_iterator end()   const;

    /// @brief Get the string representation of the ArrayRef.
    /// @example ArrayRef<int> arr : {1, 2, 3, 4, 5};
    ///          arr.to_string : "1 2 3 4 5";
    std::string to_string() const;

    /// @brief Convert ArrayRef<old_type> to ArrayRef<new_type>.
    /// @tparam New underlying data type
    /// @return ArrayRef with the modified type
    template <typename new_type>
    ArrayRef<new_type> as() const;

    /// @brief Slice the current ArrayRef.
    ArrayRef<dtype> slice(Slice const&) const;
};

/// @brief Make an empty ArrayRef.
/// @param numel The number of elements
/// @return New ArrayRef
template <typename dtype>
ArrayRef<dtype> make_array(int64_t numel);

/// @brief Make an ArrayRef filled with value.
/// @param numel The number of elements
/// @param value Used to initialize the elements of the ArrayRef
/// @param expand Indicate whether to expand the ArrayRef by repeating the value(default: false)
/// @return New ArrayRef
template <typename dtype>
ArrayRef<dtype> make_array(dtype const& value, int64_t numel, bool expand=false);

/// @brief Create an ArrayRef object from a std::vector<dtype>.
/// @param vec Containing elements to be assigned to the ArrayRef
/// @return New ArrayRef
template <typename dtype>
ArrayRef<dtype> make_array(std::vector<dtype> vec);

/// @brief Create an ArrayRef object from a std::initializer_list<dtype>.
/// @param list Containing elements to be assigned to the ArrayRef
/// @return New ArrayRef
template <typename dtype>
ArrayRef<dtype> make_array(std::initializer_list<dtype> list);

}
