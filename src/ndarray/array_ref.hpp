#pragma once

#include "array_ref.h"

#include <span>
#include <functional>
#include <type_traits>
#include <concepts>
#include <cassert>
#include <ranges>

namespace core
{

/******************************** factory ********************************/

/// @brief Make an empty ArrayRef.
/// @param numel The number of elements
/// @return New ArrayRef
template <typename dtype>
ArrayRef<dtype> make_array(int64_t numel)
{
    auto buffer = std::make_shared< typename ArrayRef<dtype>::BufferType >( numel );
    auto stride = 1;
    auto offset = 0;
    return { std::move(buffer), numel, stride, offset };
}

/// @brief Make an ArrayRef filled with value.
/// @param numel The number of elements
/// @param value Used to initialize the elements of the ArrayRef
/// @param expand Indicate whether to expand the ArrayRef by repeating the value(default: false)
/// @return New ArrayRef
template <typename dtype>
ArrayRef<dtype> make_array(dtype const& value, int64_t numel, bool expand)
{
    if(expand)
    {
        auto    buffer = std::make_shared< typename ArrayRef<dtype>::BufferType >(numel, value);
        int64_t offset = 0;
        int64_t stride = 1;
        return { std::move(buffer), numel, stride, offset };
    }
    else
    {
        auto    buffer = std::make_shared< typename ArrayRef<dtype>::BufferType >(1, value);
        int64_t offset = 0;
        int64_t stride = 0;
        return { std::move(buffer), numel, stride, offset };
    }
}

/// @brief Create an ArrayRef object from a std::vector<dtype>.
/// @param vec Containing elements to be assigned to the ArrayRef
/// @return New ArrayRef
template <typename dtype>
ArrayRef<dtype> make_array(std::vector<dtype> vec)
{
    int64_t numel = static_cast<int64_t>(vec.size());
    int64_t stride = 1;
    int64_t offset = 0;
    auto    buffer = std::make_shared< typename ArrayRef<dtype>::BufferType >( std::move(vec) );
    return { std::move(buffer), numel, stride, offset };
}

/// @brief Create an ArrayRef object from a std::initializer_list<dtype>.
/// @param list Containing elements to be assigned to the ArrayRef
/// @return New ArrayRef
template <typename dtype>
ArrayRef<dtype> make_array(std::initializer_list<dtype> list)
{
    int64_t numel  = static_cast<int64_t>(list.size());
    int64_t stride = 1;
    int64_t offset = 0;
    auto    buffer = std::make_shared< typename ArrayRef<dtype>::BufferType >( list );
    return { std::move(buffer), numel, stride, offset };
}

/******************************** array ops ********************************/

/// @brief Convert ArrayRef<old_type> to ArrayRef<new_type>.
/// @tparam New underlying data type
/// @return ArrayRef with the modified type
template <typename old_type>
template <typename new_type>
ArrayRef<new_type> ArrayRef<old_type>::as() const
{
    if( this->stride() == 0 )
    {
        auto buffer = std::make_shared< ArrayRef<new_type>::BufferType >(1, static_cast<new_type>( this->operator[](0) ));
        auto numel = this->numel();
        auto stride = 0;
        auto offset = 0;
        return { std::move(buffer), numel, stride, offset };
    }

    auto buffer = std::make_shared< ArrayRef<new_type>::BufferType >(numel);
    auto data = buffer->data();
    auto numel  = this->numel();
    auto stride = 1;
    auto offset = 0;

    auto iter = this->begin();
    for(int64_t i = 0; i < numel; ++i, ++iter)
    {
        data[i] = static_cast<new_type>(*iter);
    }

    return { std::move(buffer), numel, stride, offset };
}

/// @brief Get the string representation of the ArrayRef.
/// @example ArrayRef<int> arr : {1, 2, 3, 4, 5};
///          arr.to_string : "1 2 3 4 5";
template <typename dtype>
std::string ArrayRef<dtype>::to_string() const
{
    using namespace std::ranges::views;
    auto to_string = [](auto const& x) {
        if constexpr( requires{x.to_string();} ) {
            return x.to_string();
        }
        else{
            return std::to_string(x);
        }
    };
    // Converts array elements to strings and concatenates them into a string separated by Spaces.
    std::string str = "" + iota(0, numel()) + transform([this](int64_t i) { return this->operator[](i); }) + transform([&to_string](auto const &x) { return to_string(x); }) + " ";
    return str;
}

/******************************** iterator ********************************/

/// @brief Get the start position of the iterator.
template <typename dtype>
LNIterator<dtype> ArrayRef<dtype>::begin()
{
    return LNIterator<dtype>{
        this->data(),
        this->offset(),
        0,
        this->stride()
    };
}

/// @brief Get the start position of the const_iterator.
template <typename dtype>
LNIterator<dtype const> ArrayRef<dtype>::begin() const
{
    return LNIterator<dtype const>{
        this->data(),
        this->offset(),
        0,
        this->stride()
    };
}

/// @brief Get the end position of the iterator.
template <typename dtype>
LNIterator<dtype> ArrayRef<dtype>::end()
{
    return LNIterator<dtype>{
        this->data(), {},
        this->numel(), {}
    };
}

/// @brief Get the end position of the const_iterator.
template <typename dtype>
LNIterator<dtype const> ArrayRef<dtype>::end() const
{
    return LNIterator<dtype const>{
        this->data(), {},
        this->numel(), {}
    };
}

} //namespace core
