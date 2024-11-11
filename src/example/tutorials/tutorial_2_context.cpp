#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <cstdlib>
#include <math.h>

#include "tutorial_1_prepare.cpp"

// Use global variable to define context
pppu::Context* ctx;

namespace tutorial {

// Define value type
// In Semi2k, the first and the second data type is the same
// K example = 128
// Signed example = true, true means it is signed, false means unsigned
using Value128 = pppu::Value<mpc::Semi2k, Z2<128, true>, Z2<128, true>>;

// Set the visibility of Value, use only when using make_private
template <typename ValueType>
ValueType make_visibility(pppu::Context* ctx, ValueType res, pppu::Visibility vis) {
    if(vis.is_private()) return res;
    else if(vis.is_share()) {return pppu::input(ctx, res);} 
    else if(vis.is_public()) {auto tmp = pppu::input(ctx, res); return open(ctx, tmp);}
}

// Make a Value object
// We have two input types, intergral or floating_point, the content inside the annotation is specific to floating-point numbers
// Or you can use pppu::make_public to make public Value
template </*std::integral*/ std::floating_point DataType , typename ValueType>
ValueType make_value(pppu::Context* ctx, std::size_t pid, std::vector<DataType> data, pppu::Visibility vis/*, fracbits*/) {
    // Define the owner of data or trusted party
    playerid_t ALICE;
    if(vis.owner() != -1) ALICE = vis.owner(); else ALICE = 0;

    // Generate Value for each player separately and return
    if(pid == ALICE) {
        // Make NDArray
        core::NDArrayRef<DataType> z = core::make_ndarray(data);
        auto res = pppu::make_private<ValueType>(ctx, z/*, fracbits*/);
        return make_visibility<ValueType>(ctx, res, vis);
    } else {
        auto res = pppu::make_private<ValueType>(ctx, ALICE);
        return make_visibility<ValueType>(ctx, res, vis);
    }
}
template <std::integral  DataType , typename ValueType>
ValueType make_value(pppu::Context* ctx, std::size_t pid, std::vector<DataType> data, pppu::Visibility vis/*, fracbits*/) {
    // Define the owner of data or trusted party
    playerid_t ALICE;
    if(vis.owner() != -1) ALICE = vis.owner(); else ALICE = 0;

    // Generate Value for each player separately and return
    if(pid == ALICE) {
        // Make NDArray
        core::NDArrayRef<DataType> z = core::make_ndarray(data);
        auto res = pppu::make_private<ValueType>(ctx, z/*, fracbits*/);
        return make_visibility<ValueType>(ctx, res, vis);
    } else {
        auto res = pppu::make_private<ValueType>(ctx, ALICE);
        return make_visibility<ValueType>(ctx, res, vis);
    }
}

void tutorial_2_context(int pid, int num_parties) {
    // Prepare context
    auto context = run_player(pid, num_parties);
    ctx = context.get();

    // Prepare data
    std::vector<int> input = {1, 2, 3, 4, 5, 6, 7, 8};

    // Make Value
    auto value = make_value<int, Value128>(ctx, pid, input, pppu::Visibility::Public());

    // Performing operation : add_pp
    auto result = pppu::add(ctx, value, value);

    // Print result
    if(pid == 0) {
        auto output = pppu::detail::decode([](auto x){return int(x);}, result.data_p());
        std::cout<<output.to_string()<<std::endl;
    }
}

}
