#pragma once

#include <type_traits>

namespace core
{
    template <typename dtype> class ArrayRef;
    template <typename dtype> class NDArrayRef;
}

/// @brief isArrayRefImpl is used to determine whether the type is core::ArrayRef<dtype>.
template <typename T>
struct isArrayRefImpl: std::false_type {};

template <typename dtype>
struct isArrayRefImpl< core::ArrayRef<dtype> >: std::true_type {};

/// @brief isNDArrayRefImpl is used to determine whether the type is core::NDArrayRef<dtype>.
template <typename T>
struct isNDArrayRefImpl: std::false_type {};

template <typename dtype>
struct isNDArrayRefImpl< core::NDArrayRef<dtype> >: std::true_type {};

/// @brief The value of isArrayRefImpl<T>, true or false.
template <typename T>
concept isArrayRef = isArrayRefImpl<T>::value;

/// @brief The value of isNDArrayRefImpl<T>, true or false.
template <typename T>
concept isNDArrayRef = isNDArrayRefImpl<T>::value;
