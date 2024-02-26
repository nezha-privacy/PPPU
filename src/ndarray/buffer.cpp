#include "buffer.hpp"

namespace core
{
/// @brief Return an iterator to the start of the buffer.
/// @return Iterator to the start of the buffer
Buffer<bool>::iterator       Buffer<bool>::begin()        { return iterator(this->data(), 0); }

/// @brief Return an iterator to the start of the buffer(read-only).
/// @return Iterator to the start of the buffer(read-only)
Buffer<bool>::const_iterator Buffer<bool>::begin()  const { return const_iterator(this->data(), 0); }

/// @brief Equal const_iterator begin() const.
/// @return Const iterator to the start of the buffer
Buffer<bool>::const_iterator Buffer<bool>::cbegin() const { return const_iterator(this->data(), 0); }

/// @brief Return an iterator to the end of the buffer.
/// @return Iterator to the end of the buffer
Buffer<bool>::iterator       Buffer<bool>::end()          { return iterator(this->data(), this->size()); }

/// @brief Return an iterator to the end of the buffer(read-only).
/// @return Iterator to the end of the buffer(read-only)
Buffer<bool>::const_iterator Buffer<bool>::end()    const { return const_iterator(this->data(), this->size()); }

/// @brief Equal const_iterator end() const.
/// @return Const iterator to the end of the buffer
Buffer<bool>::const_iterator Buffer<bool>::cend()   const { return const_iterator(this->data(), this->size()); }

} // namspace core
