#pragma once

#include <span>
#include <cstdlib>
#include <type_traits>

#include "concepts.h"
#include "exceptions.h"

#include "../tools/byte_vector.h"

namespace detail {

/// @class DeserializerImpl
/// @brief The implementation of Deserializer.
template <typename DR>
class DeserializerImpl {

protected:

    using size_type = std::size_t;

    ByteVector _src;       // data source
    size_type  _head;      // read head of _src

    DeserializerImpl()                                   = delete;
    ~DeserializerImpl()                                  = default;
    DeserializerImpl(DeserializerImpl&&)                 = default;
    DeserializerImpl(const DeserializerImpl&)            = delete;
    DeserializerImpl& operator=(DeserializerImpl&&)      = default;
    DeserializerImpl& operator=(const DeserializerImpl&) = delete;

    DeserializerImpl(ByteVector&& src):
        _src(std::move(src)), _head(0) {}

    /// @brief Load n bytes from _src into data.
    void _read(void* data, size_type n) {
        if(_head + n > _src.size())
            throw deserialization_error{};

        memcpy(data, _src.data() + _head, n);
        _head += n;
    }

    /// @brief Implementation of serialization for a custom object.
    /// @param x The ByteVector object input pointer
    template <TriviallySerializable T>
    void _deserialize_trivial(T& x) {
        this->_read(&x, sizeof(x));
    }

    /// @brief Implementation of serialization for a custom object.
    /// @param x The ByteVector object input pointer
    template <CustomSerializableObject T>
    void _deserialize_custom_object(T& x) {
        if constexpr ( HasMethodArchive<T> ) {
            x.archive( *static_cast<DR*>(this) );
        } else if constexpr ( HasMethodDeserialize<T> ) {
            x.deserialize( *static_cast<DR*>(this) );
        } else if constexpr ( HasFunctionArchive<T> ) {
            archive( *static_cast<DR*>(this), x );
        } else if constexpr ( HasFunctionDeserialize<T> ) {
            deserialize( *static_cast<DR*>(this), x );
        } else {
            throw unserializable_type<T>{};
        }
    }

    /// @brief Implementation of serialization for a custom c_array.
    /// @param arr The ByteVector array input pointer
    template <CustomSerializableArray T>
    void _deserialize_custom_array(T& arr) {
        if constexpr ( std::rank_v<T> == 1 ) {
            for(auto& x: arr)
                this->_deserialize_custom_object(x);
        } else {
            for(auto& subarr: arr)
                // multi-dimension array
                this->_deserialize_custom_array(subarr);
        }
    }

    /// @brief Forward function to deserialization for a custom object or a c_array.
    /// @param x The ByteVector object input pointer
    template <CustomSerializable T>
    void _deserialize_custom(T& x) {
        if constexpr ( CustomSerializableObject<T> ) {
            this->_deserialize_custom_object(x);
        } else if constexpr( CustomSerializableArray<T> ) {
            this->_deserialize_custom_array(x);
        } else {
            throw unserializable_type<T>{};
        }
    }

    /// @brief Forward function to deserialization for a pointer or a dynamic array.
    /// @note If cannot span, throw unserializable_type exception.
    /// @param s The ByteVector object pointer need to span
    template <typename T, size_t N>
    void _deserialize_span(std::span<T, N> s) {
        if constexpr ( CustomSerializable<T> ) {
            for(auto& x: s)    this->_deserialize_custom(x);
        } else if constexpr ( TriviallySerializable<T> ) {
            this->_read(s.data(), s.size_bytes());
        } else {
            throw unserializable_type<T>{};
        }
    }

    /// @brief Restoring Object Information from Byte Sequences.
    /// @param x The ByteVector object input pointer
    template <typename T>
    void _deserialize(T&& x) {
        using value_type = std::remove_cvref_t<T>;

        if constexpr ( CustomSerializable<value_type> ) {
            this->_deserialize_custom( std::forward<T>(x) );
        } else if constexpr ( TriviallySerializable<value_type> ) {
            this->_deserialize_trivial( std::forward<T>(x) );
        } else if constexpr ( __is_std_span__<value_type>::value ) {
            this->_deserialize_span( std::forward<T>(x) );
        } else {
            throw unserializable_type<T>{};
        }
    }

    /// @brief Get the stored data of deserialized type T object.
    /// @return The stored data of deserialized type T object
    template <typename T>
    T _get() {
        if constexpr ( HasFunctionDeserializeGet<T> ) {
            return deserialize_get<T>( *static_cast<DR*>(this) );
        }
        else if constexpr ( std::is_default_constructible_v<T> ) {
            T ans;
            this->_deserialize(ans);
            return ans;
        }
        else {
            throw unserializable_type<T>{};
        }
    }

};

} // namespace datail
