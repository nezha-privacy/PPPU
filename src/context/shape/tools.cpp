#include "tools.h"

#include <algorithm>

namespace pppu::detail
{

/// @brief Create stages for odd-even merge sort.
/// @param n The number of elements of the array to be sorted
/// @return The n log^2(n) stages which odd-even merge sort operation needs
std::vector<OddEvenSortStage> OddEvenSortStages(int64_t n)
{
    std::vector<OddEvenSortStage> stages;
    for(int64_t p = 1; p < n; p <<= 1)
    {
        for(int64_t k = p; k >= 1; k >>= 1)
        {
            stages.emplace_back(n, p, k);
        }
    }
    return stages;
}

/// @brief Create the sequence for odd-even merge sort.
/// @param n The stages which odd-even merge sort operation needs
/// @return The sequence which odd-even merge sort operation needs
std::tuple<
    std::vector<int64_t>, std::vector<int64_t>
> OddEvenSortSequence(OddEvenSortStage stage) {
    std::vector<int64_t> s1;
    std::vector<int64_t> s2;

    int64_t n = stage.n;
    int64_t p = stage.p;
    int64_t k = stage.k;

    for(int64_t j = k%p; j < n-k; j += 2*k)
    {
        for(int64_t i = 0; i < std::min(k, n-j-k); i++)
        {
            if( (i+j)/(2*p) == (i+j+k)/(2*p) )
            {
                s1.push_back(i+j);
                s2.push_back(i+j+k);
            }
        }
    }

    return { s1, s2 };
}

} // namespace pppu::detail
