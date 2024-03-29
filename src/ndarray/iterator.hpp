#pragma once

#include <vector>
#include <cstdlib>

#include "util.hpp"

namespace core
{

/// @brief N-Dim array iterator.
template <typename dtype>
class NDIterator
{
protected:

    dtype* _data;
    int64_t _pos;                     // position of current element in memory
    int64_t _linear_index;            // linear index of current element
    std::vector<int64_t> _index;      // n-dimensional index of current element
    std::vector<int64_t> _shape;      // shape of the n-dimensional array
    std::vector<int64_t> _increment;  // generated by detail::makeLinearIncrement

public:

    NDIterator(dtype* data, int64_t pos, int64_t linear_index, std::vector<int64_t> index, std::vector<int64_t> shape, std::vector<int64_t> increment)
        : _data(data), _pos(pos), _linear_index(linear_index), _index( std::move(index) ),
          _shape( std::move(shape) ), _increment( std::move(increment) ) {}

    /// @brief Retrieve the value to which the iterator points.
    /// @return The value to which the iterator points
    dtype& operator*() { return _data[ _pos ]; }

    /// @brief Determine whether two iterators are equal.
    /// @param other The other iterator
    /// @return False when they are equal, true when not
    bool operator!=(NDIterator const& other) const
    {
        return this->_linear_index != other._linear_index;
    }

    /// @brief Point to the next element.
    /// @return The next element
    NDIterator& operator++()
    {
        // increase linear index
        _linear_index += 1;

        // increase N-dim index
        auto carry = detail::indexIncerment(_index, _shape);

        // update position of current element according to the position where N-dim index carry ended
        if(carry >= 0)  _pos += _increment[carry];
        else            _pos = -1;
        return *this;
    }

    /// @brief Return position of current element in memory.
    /// @return Position of current element in memory
    int64_t pos() const { return _pos; }

    /// @brief Return linear index of current element.
    int64_t linear_index() const { return _linear_index; }

    /// @brief Return n-dimensional index of current element.
    /// @return n-dimensional index of current element
    std::vector<int64_t> const& index() const { return _index; }
};

/// @brief Linear array iterator.
template <typename dtype>
class LNIterator
{
protected:

    dtype*  _data;
    int64_t _pos;     // position of current element in memory
    int64_t _index;   // linear index of current element
    int64_t _stride;  // difference of two adjacent elements' positions

public:
    LNIterator(dtype* data, int64_t pos, int64_t linear_index, int64_t stride)
        : _data(data), _pos(pos), _index(linear_index), _stride(stride) {}

    /// @brief Retrieve the value to which the iterator points.
    /// @return The value to which the iterator points
    dtype& operator*() { return _data[_pos]; }

    /// @brief Point to the next element.
    /// @return The next element
    LNIterator& operator++()
    {
        _pos += _stride;
        _index += 1;
        return *this;
    }

    /// @brief Determines whether two iterators are equal.
    /// @param other The other iterator
    /// @return False when they are equal, true when not
    bool operator!=(LNIterator const& other) const { return this->_index != other._index; }

    /// @brief Return position of current element in memory.
    /// @return Position of current element in memory
    int64_t pos() const { return _pos; }

    /// @brief Return linear index of current element.
    /// @return n-dimensional index of current element
    int64_t linear_index() const { return _index; }
};

} // namspace core
