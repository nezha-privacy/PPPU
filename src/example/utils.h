#pragma once

#include <cstdlib>
#include <random>
#include <string.h>
#include <tuple>
#include <vector>

#include "context/visibility.h"
#include "context/value.h"
#include "context/value.hpp"
#include "context/basic/factory.h"
#include "context/basic/factory.hpp"
#include "context/basic/basic.hpp"
#include "context/basic/util.hpp"
#include "datatypes/Z2k.h"
#include "datatypes/Z2k.hpp"
#include "mpc/protocol.hpp"
#include "mpc/preprocessing.hpp"
#include "mpc/semi2k/semi2k.hpp"
#include "network/network.hpp"
#include "network/multi_party_player.h"
#include "network/multi_party_player.hpp"
#include "ndarray/ndarray_ref.h"
#include "ndarray/ndarray_ref.hpp"
#include "serialization/serialization.hpp"

std::unique_ptr<network::MultiPartyPlayer> make_netio(
    playerid_t pid,
    std::size_t num_parties,
    std::string ssl_dir
);

template <typename ProtocolType, typename ProtocolTriple>
std::tuple<std::unique_ptr<mpc::Protocol>, std::unique_ptr<mpc::Preprocessing>> make_protocol(
    network::MultiPartyPlayer* netio
);

pppu::Config make_config(
    int64_t fxp_security_parameter,
    int64_t fxp_fracbits
);

std::shared_ptr<pppu::Context> make_context(
    pppu::Config config,
    std::unique_ptr<mpc::Protocol> prot,
    std::unique_ptr<mpc::Preprocessing> prep,
    std::unique_ptr<network::MultiPartyPlayer> netio
);

int rand_32();

template <typename DataType>
core::NDArrayRef<DataType> make_ndarray(int length);

template <typename ValueType>
ValueType make_visibility(pppu::Context* ctx, ValueType res, pppu::Visibility vis);

template <std::integral DataType, typename ValueType>
ValueType make_value(pppu::Context* ctx, std::size_t pid, int length, pppu::Visibility vis);

template <std::floating_point DataType, typename ValueType>
ValueType make_value(pppu::Context* ctx, std::size_t pid, int length, pppu::Visibility vis, int64_t fracbits = -1);