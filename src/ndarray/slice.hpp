#pragma once

#include <cstdint>
#include <string>
#include <cassert>
#include <optional>
#include <numeric>
#include <variant>
#include <vector>

namespace core
{

/// @brief Represents a slice with optional start, stop, and step values.
/// @note Allow negative start and stop (like python).
/// @note Step default 1; if step > 0, start default 0, stop default n,
///                       if step < 0, start default n-1, stop default -1.
struct Slice
{
    std::optional<int64_t> start;
    std::optional<int64_t> stop;
    std::optional<int64_t> step;

    /// @brief Convert the Slice object to a string representation "{start}:{stop}:{step}".
    std::string to_string() const;
};

/// @brief Whether two slices are equal.
/// @note Input should be normalized.
bool operator==(Slice const& lhs, Slice const& rhs);

namespace detail {

/// @brief Calculate number of elements included by slicing.
/// @param n The length of array
/// @return Number of elements included by slicing
/// @note Input should be normalized.
int64_t calcNumel(Slice const&, int64_t n);

/// @brief Modify start and stop using positive index or default value.
/// @param slice Input reference and modify its value to normalize
/// @param n The length of array
/// @return Slice to be normalized.
/// @note Check for empty slice and non-0 step.
Slice normalize(Slice const&, int64_t n);

/// @brief Modify start and stop of slice using positive index or default value, modify int64_t using positive index.
/// @param indicies Input reference and modify its value to normalize, default to full slicing
/// @param shape Array consisting of the length of the array
/// @return Unified representation using normalized slices and int64_t
std::vector<std::variant<Slice, int64_t>> normalize(
    std::vector<std::variant<Slice, int64_t>> const &indicies,
    std::vector<int64_t> const &shape);

} // namespace detail

} // namespace core
