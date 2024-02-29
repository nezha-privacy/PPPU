#pragma once

#include <type_traits>
#include "example/utils.h"

using tcp = boost::asio::ip::tcp;
using address = boost::asio::ip::address;

template <typename ProtocolType, typename ProtocolTriple>
std::tuple<std::unique_ptr<mpc::Protocol>, std::unique_ptr<mpc::Preprocessing>> make_protocol(
    network::MultiPartyPlayer* netio
) {
    std::unique_ptr<mpc::Protocol> prot;
    std::unique_ptr<mpc::Preprocessing> prep;

    if(netio==nullptr)
        throw std::invalid_argument("must provide valid netio");

    prep = std::make_unique<ProtocolTriple>();
    prot = std::make_unique<ProtocolType>(netio, prep.get());

    return std::make_tuple(std::move(prot), std::move(prep));
}

template <typename DataType>
core::NDArrayRef<DataType> make_ndarray(int length) {
    std::vector<DataType> x;
    for(int i = 0; i < length; i++) {
        DataType y = (DataType)rand_32();
        x.push_back(y);
    }
    core::NDArrayRef<DataType> z = core::make_ndarray(x);
    return z;
}

template <typename DataType>
core::NDArrayRef<DataType> make_ndarray_data(DataType data) {
    std::vector<DataType> x;
    x.push_back(data);
    core::NDArrayRef<DataType> z = core::make_ndarray(x);
    return z;
}

template <typename ValueType>
ValueType make_visibility(pppu::Context* ctx, ValueType res, pppu::Visibility vis) {
    if(vis.is_private()) return res;
    else if(vis.is_share()) {return pppu::input(ctx, res);} 
    else if(vis.is_public()) {auto tmp = pppu::input(ctx, res); return open(ctx, tmp);}
}

template <std::integral DataType, typename ValueType>
ValueType make_value(pppu::Context* ctx, std::size_t pid, int length, pppu::Visibility vis) {
    playerid_t ALICE;
    if(vis.owner() != -1) ALICE = vis.owner(); else ALICE = 0;
    if(pid == ALICE) {
        core::NDArrayRef<DataType> z = make_ndarray<DataType>(length);
        auto res = pppu::make_private<ValueType>(ctx, z);
        return make_visibility<ValueType>(ctx, res, vis);
    } else {
        auto res = pppu::make_private<ValueType>(ctx, ALICE);
        return make_visibility<ValueType>(ctx, res, vis);
    }
}

template <std::floating_point DataType, typename ValueType>
ValueType make_value(pppu::Context* ctx, std::size_t pid, int length, pppu::Visibility vis, int64_t fracbits = -1) {
    playerid_t ALICE;
    if(vis.owner() != -1) ALICE = vis.owner(); else ALICE = 0;
    if(pid == ALICE) {
        core::NDArrayRef<DataType> z = make_ndarray<DataType>(length);
        auto res = pppu::make_private<ValueType>(ctx, z, fracbits);
        return make_visibility<ValueType>(ctx, res, vis);
    } else {
        auto res = pppu::make_private<ValueType>(ctx, ALICE);
        return make_visibility<ValueType>(ctx, res, vis);
    }
}

template <std::integral DataType, typename ValueType>
ValueType make_value_data(pppu::Context* ctx, std::size_t pid, DataType data, pppu::Visibility vis) {
    playerid_t ALICE;
    if(vis.owner() != -1) ALICE = vis.owner(); else ALICE = 0;
    if(pid == ALICE) {
        core::NDArrayRef<DataType> z = make_ndarray_data<DataType>(data);
        auto res = pppu::make_private<ValueType>(ctx, z);
        return make_visibility<ValueType>(ctx, res, vis);
    } else {
        auto res = pppu::make_private<ValueType>(ctx, ALICE);
        return make_visibility<ValueType>(ctx, res, vis);
    }
}

template <std::floating_point DataType, typename ValueType>
ValueType make_value_data(pppu::Context* ctx, std::size_t pid, DataType data, pppu::Visibility vis, int64_t fracbits = -1) {
    playerid_t ALICE;
    if(vis.owner() != -1) ALICE = vis.owner(); else ALICE = 0;
    if(pid == ALICE) {
        core::NDArrayRef<DataType> z = make_ndarray_data<DataType>(data);
        auto res = pppu::make_private<ValueType>(ctx, z, fracbits);
        return make_visibility<ValueType>(ctx, res, vis);
    } else {
        auto res = pppu::make_private<ValueType>(ctx, ALICE);
        return make_visibility<ValueType>(ctx, res, vis);
    }
}

template <typename DataType, typename ValueType>
ValueType make_value_vec(pppu::Context* ctx, std::size_t pid, DataType data, pppu::Visibility vis, int64_t fracbits = -1) {
    playerid_t ALICE;
    if(vis.owner() != -1) ALICE = vis.owner(); else ALICE = 0;
    if(pid == ALICE) {
        core::NDArrayRef<double> z = core::make_ndarray(data);
        auto res = pppu::make_private<ValueType>(ctx, z, fracbits);
        return make_visibility<ValueType>(ctx, res, vis);
    } else {
        auto res = pppu::make_private<ValueType>(ctx, ALICE);
        return make_visibility<ValueType>(ctx, res, vis);
    }
}
