#pragma once

#include "serializer_impl.h"

/// @class Serializer
/// @brief Responsible for converting C++objects into byte sequences.
/// @details When instantiating a Serializer object, the default constructor should 
///          be used, which silently creates a ByteVector class object where all serialized data 
///          is stored. When serialized data needs to be sent, the finalize() method of the Serializer 
///          object should be called, which returns the right value reference of the byte sequence.
/// @note Curiously recurring template pattern.
class Serializer:
public detail::SerializerImpl<Serializer> {

    using super = detail::SerializerImpl<Serializer>;

public:

    Serializer()                             = default;
    ~Serializer()                            = default;
    Serializer(Serializer&&)                 = delete;
    Serializer(const Serializer&)            = delete;
    Serializer& operator=(Serializer&&)      = delete;
    Serializer& operator=(const Serializer&) = delete;

    template <typename T>
    Serializer& operator&(const T& x) {
        return this->operator<<(x);
    }

    template <typename T>
    Serializer& operator<<(const T& x) {
        return this->serialize(x);
    }

    /// @brief Responsible for converting C++objects into byte sequences.
    template <typename T>
    Serializer& serialize(const T& x) {
        super::_serialize(x);
        return *this;
    }

    /// @brief Returns the right value reference of a byte sequence.
    /// @return The right value reference of a byte sequence
    ByteVector&& finalize() {
        return super::_finalize();
    }

};
