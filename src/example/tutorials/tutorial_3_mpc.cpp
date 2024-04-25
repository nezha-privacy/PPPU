#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <cstdlib>
#include <math.h>

#include "tutorial_1_prepare.cpp"

namespace tutorial {

// Define Z datatype, explained in tutorial_2_context
using Z = Z2<128, true>;

void tutorial_3_mpc(int pid, int num_parties) {
    // prepare network
    auto netio = make_netio(pid, num_parties, "");
    auto player = std::move(netio).get();
    mpc::Semi2kTriple semi2k_triple;
    mpc::Semi2k semi2k(pid, num_parties, player, &semi2k_triple);

    // Prepare data
    std::vector<Z> input = {1, 2, 3, 4, 5, 6, 7, 8};

    // Make Arrayref
    core::ArrayRef<Z> arr = core::make_array(input);

    // Performing operation : add_pp
    auto result = semi2k.add_pp(arr, arr);

    // Print result
    if(pid == 0) {
        for(std::size_t i = 0; i != result.numel(); ++i) {
            std::cout<<result[i].to_string();
            if(i < result.numel() - 1) std::cout<<" ";
        }
    }
}

}
