#pragma once

#include <concepts>
#include <type_traits>

#include "deserializer_impl.h"

/// @class Deserializer
/// @brief Restoring Object Information from Byte Sequences.
/// @note When creating a Deserializer object, the constructor accepts a formal parameter type of ByteVector&&.
class Deserializer:
public detail::DeserializerImpl<Deserializer> {

private:

    using super = detail::DeserializerImpl<Deserializer>;

public:

    Deserializer()                               = delete;
    ~Deserializer()                              = default;
    Deserializer(Deserializer&&)                 = default;
    Deserializer(const Deserializer&)            = delete;
    Deserializer& operator=(Deserializer&&)      = default;
    Deserializer& operator=(const Deserializer&) = delete;

    Deserializer(ByteVector&& src): super(std::move(src)) {}

    template <typename T>
    Deserializer& operator&(T&& x) {
        return this->operator>>( std::forward<T>(x) );
    }

    template <typename T>
    Deserializer& operator>>(T&& x) {
        return this->deserialize( std::forward<T>(x) );
    }

    /// @brief Restoring Object Information from Byte Sequences.
    template <typename T>
    Deserializer& deserialize(T&& x) {
        super::_deserialize( std::forward<T>(x) );
        return *this;
    }

    /// @brief Get the stored data of deserialized type T object.
    /// @return The stored data of deserialized type T object
    template <typename T>
    T get() {
        return super::_get<T>();
    }

};
