#include "permutation.hpp"

#include <ranges>
#include <bit>
#include <functional>
#include <fmt/format.h>

#include "math.h"

/// @brief Obtain the inverse permutation of a specific permutation.
/// @param perm The input permutation
/// @return The inverse permutation of a specific permutation
std::vector<int64_t> inv(std::vector<int64_t> const &perm)
{
    std::vector<int64_t> ans(perm.size());
    for (std::size_t i = 0; i < perm.size(); ++i)
        ans[perm[i]] = i;
    return ans;
}

/// @brief Obtain the signals of the switches of the Bernes permutation network for the input permutation, 1 for switch.
/// @note Posible optimization point: Use non-recursive algorithm. Avoid frequent slicing (variant and optional may incur heavy cost).
///       performance:
///       perm size  non-optimized  optimized(O3)
///       2^16        0.67s            0.06s
///       2^20        12s               1.2s
///       2^24                          21s
/// @param perm The input permutation
/// @return The signals for each unit in benes network, 1 for switch
core::NDArrayRef<bool> benes(std::vector<int64_t> const& perm) {

    int64_t N = perm.size();
    if(N <= 1 || !is_pow2(N)) {
        throw std::invalid_argument("size of the permutation must be power of 2");
    }

    // initialize the signal matrix of shape (N/2, 2logN - 1) and value -1
    // then create a reference of the matrix for recursive configuration
    int64_t logN = static_cast<int64_t>(log2(N));

    auto signals = core::zeros<bool>({N/2, 2*logN-1});;

    // define the recursive function for configuring the signals of switching network
    std::function<void(const std::vector<int64_t>&, decltype(signals)&)> config;
    config = [&config](const std::vector<int64_t>& perm, decltype(signals)& signals){
        int64_t N    = perm.size();
        int64_t logN = static_cast<int64_t>(log2(N));

        // base case, N = 2
        if(N == 2) {
            assert( signals.numel() == 1 );
            signals.elem({0, 0}) = perm[0];
            return;
        }

        // recursive case, N >= 4
        std::vector<int64_t> invp = inv(perm);

        // whether O[i] has been visited or not
        std::vector<bool> vis(N/2, false);

        auto I = signals.slice( { core::Slice{ .start=0,   .stop=N/2 },  0 } );
        auto O = signals.slice( { core::Slice{ .start=0,   .stop=N/2 }, -1 } );
        auto U = signals.slice( { core::Slice{ .start=0,   .stop=N/4 }, core::Slice{ .start=1, .stop=-1 } } );
        auto D = signals.slice( { core::Slice{ .start=N/4, .stop=N/2 }, core::Slice{ .start=1, .stop=-1 } } );

        std::vector<int64_t> perm0(N/2);
        std::vector<int64_t> perm1(N/2);

        for(int64_t k = 0; k < N; k += 2) {
            if(vis[k/2] == false) {
                vis[k/2] = true;
                O.elem({k/2}) = 0;

                int64_t i, j = k;
                while(1) {
                    // set I[i/2], perm0 {i/2 -> j/2}
                    i = perm[j];
                    perm0[j/2] = i/2;
                    I.elem({i/2}) = (i&1);
                    i += (i&1 ? -1 : 1);

                    // set O[j/2], perm1 {i/2 -> j/2}
                    j = invp[i];
                    perm1[j/2] = i/2;

                    if(vis[j/2])  break;

                    vis[j/2] = true;
                    O.elem({j/2}) = (j&1)^1;
                    j += (j&1 ? -1 : 1);

                }
            }
        }
        config(perm0, U);
        config(perm1, D);
    };

    config(perm, signals);
    return signals;
}
