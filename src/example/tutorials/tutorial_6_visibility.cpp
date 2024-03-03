#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <cstdlib>
#include <math.h>

#include "tutorial_1_prepare.cpp"
#include "context/math/div.hpp"

namespace tutorial {

// Define value type
// In Semi2k, the first and the second data type is the same
// K example = 128
// Signed example = true, true means it is signed, false means unsigned
using Value = pppu::Value<mpc::Semi2k, Z2<128, true>, Z2<128, true>>;


/*
    The Value object can be divided into three privacy states (pubilic, private and share) and encapsulates all the elements needed for MPC computation.
    Data in different privacy states will be automatically converted during arithmetic,for example, public and private data will be converted into share when calculated with share.
    You can think of it as just doing normal arithmetic.
*/
void tutorial_6_visibility(int pid, int num_parties) {
    // Prepare context
    auto context = run_player(pid, num_parties);
    ctx = context.get();

    auto data = core::make_ndarray<int>({1, 2, 3, 4});
    Value private_data;
    // make_private generates a Value of type private, where the owner of the data takes the data as a parameter 
    // and others take the data owner as a parameter.
    if(ctx->pid() == 0){
    private_data = make_private<Value>(ctx, data);
    } else{
    private_data = make_private<Value>(ctx, 0);
    }

    // make_public generates a Value of type public.
    Value public_data = make_public<Value>(ctx, data);
    std::vector<int64_t> shape = {2, 3};
    // make_constant generates a Value of type public with shape size.
    Value const_data = make_constant<Value>(ctx, 1, shape);

    // visibility, is_private, is_public, is_share can judge the privacy status of Value.  
    // owner can obtain the own of private data, only the private type is supported.
    assert(private_data.visibility() == pppu::Visibility::Private(0));
    assert(private_data.owner() == 0);
    assert(private_data.is_private());
    assert(public_data.is_public());

    // input changes the type of private to share.  
    Value share_data = input(ctx, private_data);
    assert(share_data.is_share());

    // open changes the type of share to public.
    Value open_data = open(ctx, share_data);
    assert(open_data.is_public());

    // Two types of data of the same type remain the same after operation.
    // public data will be converted into private when calculated with private.
    Value add_res = pppu::add(ctx, private_data, public_data);
    assert(add_res.is_private());

    // public and private data will be converted into share when calculated with share.
    Value div_res = pppu::div(ctx, private_data, share_data);
    assert(div_res.is_share());
}

}
