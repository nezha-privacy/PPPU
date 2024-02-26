#pragma once

#include <concepts>

/// @brief Concept representing an Additive Group.
template <typename T>
concept AddtiveGroup = requires(T x, T y) {
    // element zero
    // additive inverse
    T(0);
    { + x } -> std::same_as<T>;
    { - x } -> std::same_as<T>;
    {x + y} -> std::same_as<T>;
    {x - y} -> std::same_as<T>;
};

/// @brief Concept representing a Multiplicative Group.
template <typename T>
concept MultiplicativeGroup = requires(T x, T y) {
    // element one
    // multiplicative inverse
    T(1);
    {x * y} -> std::same_as<T>;
    {x / y} -> std::same_as<T>;
};

/// @brief Concept representing a Ring.
template <typename T>
concept Ring = requires(T x, T y) {
    // element zero
    // element one
    // additive inverse
    T(0);
    T(1);
    { + x } -> std::same_as<T>;
    { - x } -> std::same_as<T>;
    {x + y} -> std::same_as<T>;
    {x - y} -> std::same_as<T>;
    {x * y} -> std::same_as<T>;
};

/// @brief Concept representing a Field.
template <typename T>
concept Field = requires(T x, T y) {
    // element zero
    // element one
    // additive inverse
    // multiplicative inverse
    T(0);
    T(1);
    { + x } -> std::same_as<T>;
    { - x } -> std::same_as<T>;
    {x + y} -> std::same_as<T>;
    {x - y} -> std::same_as<T>;
    {x * y} -> std::same_as<T>;
    {x / y} -> std::same_as<T>;
};
