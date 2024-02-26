#pragma once

#include <span>
#include <cstdlib>
#include <type_traits>

#include "concepts.h"
#include "exceptions.h"

#include "../tools/byte_vector.h"

namespace detail {

/// @class SerializerImpl
/// @brief The implementation of Serializer.
template <typename SR>
class SerializerImpl {

protected:

    using size_type = std::size_t;

    ByteVector _sink;

    SerializerImpl()                                 = default;
    ~SerializerImpl()                                = default;
    SerializerImpl(SerializerImpl&&)                 = delete;
    SerializerImpl(const SerializerImpl&)            = delete;
    SerializerImpl& operator=(SerializerImpl&&)      = delete;
    SerializerImpl& operator=(const SerializerImpl&) = delete;

    /// @brief Write {data, n} to _sink
    /// @param data The C++ object input reference
    /// @param n The number of bytes
    void _write(const void* data, size_type n) {
        _sink.push_back(data, n);
    }

    /// @brief Forward function for trivial object / c_array to write().
    /// @param x The C++ object input pointer
    template <typename T>
    void _serialize_trivial(const T& x) {
        this->_write(&x, sizeof(x));
    }

    /// @brief Implementation of serialization for a custom object.
    /// @param x The C++ object input pointer
    template <typename T>
    void _serialize_custom_object(const T& x) {
        if constexpr ( HasMethodArchive<T> ) {
            x.archive( *static_cast<SR*>(this) );
        } else if constexpr ( HasMethodSerialize<T> ) {
            x.serialize( *static_cast<SR*>(this) );
        } else if constexpr ( HasFunctionArchive<T> ) {
            archive( *static_cast<SR*>(this), x );
        } else if constexpr ( HasFunctionSerialize<T> ) {
            serialize( *static_cast<SR*>(this), x );
        } else {
            throw unserializable_type<T>{};
        }
    }

    /// @brief Implementation of serialization for a custom c_array.
    /// @param arr The C++ array input pointer
    template <typename T>
    void _serialize_custom_array(const T& arr) {

        if constexpr ( std::rank_v<T> == 1 ) {
            for(auto& x: arr)
                this->_serialize_custom_object(x);
        } else {
            for(auto& subarr: arr)
                // multi-dimension array
                this->_serialize_custom_array( subarr );
        }
    }

    /// @brief Forward function to serialization for a custom object or a c_array.
    /// @param x The C++ object input pointer
    template <typename T>
    void _serialize_custom(const T& x) {
        if constexpr ( CustomSerializableObject<T> ) {
            this->_serialize_custom_object(x);
        } else if constexpr( CustomSerializableArray<T> ) {
            this->_serialize_custom_array(x);
        } else {
            throw unserializable_type<T>{};
        }
    }

    /// @brief Forward function to serialization for a pointer or a dynamic array.
    /// @note If cannot span, throw unserializable_type exception.
    /// @param s The C++ object pointer need to span
    template <typename T, std::size_t N>
    void _serialize_span(std::span<T, N> s) {
        using value_type = std::remove_cvref_t<T>;
        if constexpr ( CustomSerializable<value_type> ) {
            for(auto const& x: s)    this->_serialize_custom(x);
        } else if constexpr ( TriviallySerializable<value_type> ) {
            this->_write(s.data(), s.size_bytes());
        } else {
            throw unserializable_type<T>{};
        }
    }

    /// @brief Converting C++objects into byte sequences.
    /// @param x The C++ object input pointer
    template <typename T>
    void _serialize(const T& x) {
        if constexpr ( CustomSerializable<T> ) {
            this->_serialize_custom(x);
        } else if constexpr ( TriviallySerializable<T> ) {
            this->_serialize_trivial(x);
        } else if constexpr ( __is_std_span__<T>::value ) {
            this->_serialize_span(x);
        } else {
            throw unserializable_type<T>{};
        }
    }

    /// @brief Returns the right value reference of a byte sequence.
    /// @return The right value reference of a byte sequence
    ByteVector&& _finalize() {
        return std::move(_sink);
    }

};

} // namespace detail
