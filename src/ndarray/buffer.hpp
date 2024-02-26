#pragma once

#include <vector>
#include <cstdint>

#include "buffer.hpp"

namespace core
{

template <typename T>
class Buffer: public std::vector<T>
{
public:
    /// @brief Inherit the constructors from std::vector<T>.
    using std::vector<T>::vector;

    /// @brief Constructs a Buffer object from an existing vector.
    /// @param vec The vector to be moved into the Buffer
    Buffer(std::vector<T> vec): std::vector<T>{ std::move(vec) } {}
};

/// @brief Undo the specilization of std::vector<bool>.
template <>
class Buffer<bool>: private std::vector<uint8_t>
{
private:
    using vector = std::vector<uint8_t>;

public:
    using value_type      = bool;
    using size_type       = std::size_t;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using pointer         = value_type*;
    using const_pointer   = value_type const*;

    Buffer()                         = default;
    ~Buffer()                        = default;
    Buffer(Buffer&&)                 = default;
    Buffer(Buffer const&)            = default;
    Buffer& operator=(Buffer&&)      = default;
    Buffer& operator=(Buffer const&) = default;

    explicit Buffer(size_type count): vector(count) {}
    Buffer(size_type count, bool value): vector(count, value) {}
    Buffer(std::initializer_list<bool> init) { for(bool value: init) this->push_back(value); }
    Buffer(std::vector<bool> const& vec) { for(bool value: vec) this->push_back(value); }

    /**************** element access ****************/
    /// @brief Access the element at the specified location.
    /// @param pos The position of element
    /// @return The element at the specified location
    reference       at        (size_type pos)       { return *reinterpret_cast<pointer>      (&vector::at(pos)); }
    const_reference at        (size_type pos) const { return *reinterpret_cast<const_pointer>(&vector::at(pos)); }

    /// @brief Access the element by subscript.
    /// @param pos The position of element
    /// @return The element by subscript
    reference       operator[](size_type pos)       { return *reinterpret_cast<pointer>      (&vector::operator[](pos)); }
    const_reference operator[](size_type pos) const { return *reinterpret_cast<const_pointer>(&vector::operator[](pos)); }

    /// @brief Access the first element.
    /// @return The first element
    reference       front     ()                    { return *reinterpret_cast<pointer>      (&vector::front()); }
    const_reference front     ()              const { return *reinterpret_cast<const_pointer>(&vector::front()); }

    /// @brief Access the last element.
    /// @return The last element
    reference       back      ()                    { return *reinterpret_cast<pointer>      (&vector::back()); }
    const_reference back      ()              const { return *reinterpret_cast<const_pointer>(&vector::back()); }

    /// @brief Return a pointer to the buffer.
    /// @return Pointer to the buffer
    pointer         data      ()                    { return  reinterpret_cast<pointer>      ( vector::data()); }
    const_pointer   data      ()              const { return  reinterpret_cast<const_pointer>( vector::data()); }

    /**************** iterators ****************/
    class iterator;
    class const_iterator;

    /// @brief Return an iterator to the start of the buffer.
    /// @return Iterator to the start of the buffer
    iterator       begin();

    /// @brief Return an iterator to the start of the buffer(read-only).
    /// @return Iterator to the start of the buffer(read-only)
    const_iterator begin()  const;

    /// @brief Equal const_iterator begin() const.
    /// @return Const iterator to the start of the buffer
    const_iterator cbegin() const;

    /// @brief Return an iterator to the end of the buffer.
    /// @return Iterator to the end of the buffer
    iterator       end();

    /// @brief Return an iterator to the end of the buffer(read-only).
    /// @return Iterator to the end of the buffer(read-only)
    const_iterator end()    const;

    /// @brief Equal const_iterator end() const.
    /// @return Const iterator to the end of the buffer
    const_iterator cend()   const;

    /**************** capacity ****************/
    using vector::empty;
    using vector::size;
    using vector::max_size;
    using vector::reserve;
    using vector::capacity;
    using vector::shrink_to_fit;

    /**************** modifiers ****************/
    using vector::clear;
    using vector::resize;
    using vector::pop_back;
    void push_back(bool value) { vector::push_back( static_cast<uint8_t>(value) ); }

};

class Buffer<bool>::iterator: public std::iterator<
    std::contiguous_iterator_tag,
    bool, long, bool*, bool&>
{
private:
    pointer data;
    std::size_t pos;
public:
    iterator(pointer data, std::size_t pos) : data(data), pos(pos) {}

    /// @brief Point to the next element.
    iterator& operator++() { pos += 1; return *this; }

    /// @brief Determine whether two iterators are equal.
    bool operator!=(iterator const& other) const {return pos != other.pos;}

    /// @brief Retrieve the value to which the iterator points.
    reference operator*() const {return data[pos];}
};

class Buffer<bool>::const_iterator: public std::iterator<
    std::contiguous_iterator_tag,
    bool, long, bool const*, bool const&>
{
private:
    pointer data;
    std::size_t pos;
public:
    const_iterator(pointer data, std::size_t pos) : data(data), pos(pos) {}

    /// @brief Point to the next element(read-only).
    const_iterator& operator++() { pos += 1; return *this; }

    /// @brief Determine whether two iterators are equal(read-only).
    bool operator!=(const_iterator const& other) const {return pos != other.pos;}
    
    /// @brief Retrieve the value to which the iterator points(read-only).
    reference operator*() const {return data[pos];}
};

} // namspace core
