#pragma once

#include <concepts>
#include <span>

class Serializer;
class Deserializer;

namespace detail
{

/**************** custom serializable ****************/

/// @brief T is required to have a member function named archive 
///        that takes a Serializer object or a Deserializer object as arguments.
template <typename T>
concept HasMethodArchive = requires(T x, Serializer s, Deserializer d) {
    x.archive(s);
    x.archive(d);
};
/// @brief T is required to have a member function named serialize 
///        that takes a Serializer object as an argument.
template <typename T>
concept HasMethodSerialize = requires(T x, Serializer s) {
    x.serialize(s);
};

/// @brief T is required to have a member function named deserialize 
///        that takes a Deserializer object as an argument.
template <typename T>
concept HasMethodDeserialize = requires(T x, Deserializer d) {
    x.deserialize(d);
};

/// @brief Support the archive function, which takes a Serializer object s or a Deserializer object,
///        as well as an object of type T as its arguments.
template <typename T>
concept HasFunctionArchive = requires(T x, Serializer s, Deserializer d) {
    archive(s, x);
    archive(d, x);
};

/// @brief Support the serialize function, which takes a Serializer object 
///        and an object of type T as its arguments.
template <typename T>
concept HasFunctionSerialize = requires(T x, Serializer s) {
    serialize(s, x);
};

/// @brief Support the serialize function, which takes a Deserializer object 
///        and an object of type T as its arguments.
template <typename T>
concept HasFunctionDeserialize = requires(T x, Deserializer d) {
    deserialize(d, x);
};

/// @brief Support the deserialize_get function, which takes a Deserializer object as its argument.
///        and return type of the deserialize_get<T>(d) expression must be the same as T.
template <typename T>
concept HasFunctionDeserializeGet = requires(Deserializer d) {
    { deserialize_get<T>(d) } -> std::same_as<T>;
};

/// @brief Check whether type T is a custom serializable object.
template <typename T>
concept CustomSerializableObject = (
     HasMethodArchive<T>     ||  HasFunctionArchive<T>    ||
    (HasMethodSerialize<T>   &&  HasMethodDeserialize<T>) ||
    (HasFunctionSerialize<T> &&  (HasFunctionDeserialize<T> || HasFunctionDeserializeGet<T>) )
);

/// @brief Check whether type T is a custom serializable array.
template <typename T>
concept CustomSerializableArray = (
    std::is_bounded_array_v<T> &&
    CustomSerializableObject< std::remove_all_extents_t<T> >
);

/// @brief Check whether a type T is custom serializable. 
template <typename T>
concept CustomSerializable = (
    CustomSerializableObject<T> ||
    CustomSerializableArray<T>
);

/****************  trivially serializable  ****************/

/// @details Trivially serializable object can be serialized by memcpy(&x, sizeof(x))
///          user defined classes must be explicitly annotated with the following declaration
///          to be trivially serializable.
///          class UserDefinedClass {
///              static constexpr bool trivially_serializable = true;
///          }
template <typename T>
concept TriviallySerializableBasicObject = (
       std::same_as<T, bool>
    || std::same_as<T, char>        || std::same_as<T, unsigned char>
    || std::same_as<T, short>       || std::same_as<T, unsigned short>
    || std::same_as<T, int>         || std::same_as<T, unsigned int>
    || std::same_as<T, long>        || std::same_as<T, unsigned long>
    || std::same_as<T, long long>   || std::same_as<T, unsigned long long>
    || std::same_as<T, float>       || std::same_as<T, double>
    || std::is_enum_v<T>
);

template <typename T>
concept TriviallySerializableCustomObject = (
    requires(T x) { T::trivially_serializable; } &&
    T::trivially_serializable
);

template <typename T>
concept TriviallySerializableObject = (
    TriviallySerializableBasicObject<T> ||
    TriviallySerializableCustomObject<T>
);

template <typename T>
concept TriviallySerializableArray = (
    std::is_bounded_array_v<T> &&
    TriviallySerializableObject< std::remove_all_extents_t<T> >
);

/// @note Why not use std::is_trivially_copyable?
///       Cause pointers, std::span, and many others are also trivially copyable
///       which are not trivial to serialize
template <typename T>
concept TriviallySerializable = (
    TriviallySerializableObject<T> ||
    TriviallySerializableArray<T>
);


/**************** serializable ****************/

/// @brief Determine whether a object is serializable or not.
template <typename T>
concept Serializable = (
    TriviallySerializable<T>
    || CustomSerializable<T>
);

/*************** span ********************/

/// @brief Need template params to implement serialize or deserialize.
template <typename T>
struct __is_std_span__: public std::false_type {};

template <typename T, std::size_t N>
struct __is_std_span__<std::span<T, N>>: public std::true_type {};

} // namespace datail
