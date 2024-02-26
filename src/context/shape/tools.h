#pragma once

#include <vector>
#include <cstdlib>
#include <tuple>

namespace pppu::detail
{


struct OddEvenSortStage
{
    int64_t n;   // total number of elements to sort
    int64_t p;   // 1, 2, 4, 8, ..., <n
    int64_t k;   // p, p/2, p/4, ..., 1
};

/// @brief Create stages for odd-even merge sort.
/// @param n The number of elements of the array to be sorted
/// @return The n log^2(n) stages which odd-even merge sort operation needs
std::vector<OddEvenSortStage>
    OddEvenSortStages(int64_t nr_elems);

/// @brief Create the sequence for odd-even merge sort.
/// @param n The stages which odd-even merge sort operation needs
/// @return The sequence which odd-even merge sort operation needs
std::tuple<std::vector<int64_t>, std::vector<int64_t>>
    OddEvenSortSequence(OddEvenSortStage stage);

} // namespace pppu::detail
