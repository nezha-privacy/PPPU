#pragma once

#include <concepts>

namespace mpc
{

/// @class Preprocessing
/// @brief A preprocessing base class. Use to contains preprocessings.
class Preprocessing
{
public:
    Preprocessing() = default;
    ~Preprocessing() = default;
};

/// @brief Judgment for instantiation.
template <typename T>
concept isPreprocessing = std::derived_from<T, Preprocessing>;

} // namespace mpc
