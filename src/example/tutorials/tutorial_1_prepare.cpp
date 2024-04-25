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

namespace tutorial {

// Execute in order

// Step 1 : Make pppu::Config object to define fxp operations methods. 
pppu::Config make_config(int64_t fxp_security_parameter, int64_t fxp_fracbits) {
    // Config
    pppu::Config conf;

    // Basic definitions
    conf.fxp_fracbits = fxp_fracbits; // Default 40
    conf.fxp_security_parameter = fxp_security_parameter; // Default 3

    // Operation modes and parameters
    conf.fxp_div_iters = 20; // Default 20
    conf.fxp_div_mode = pppu::Config::FXP_DIV_NEWTON;
    conf.fxp_sigmoid_mode = pppu::Config::FXP_SIGMOID_EULER;
    conf.fxp_exp_iters = 20;
    // exp intbits refers to the integer number of exp. 
    // When intbits is set to 6, the operation of integer bits from -32 to 31 is supported. 
    // exp can be a floating point number.
    conf.fxp_exp_intbits = 6; 
    conf.fxp_exp_mode = pppu::Config::FXP_EXP_TAYLOR;
    conf.fxp_log_iters = 20;
    conf.fxp_log_mode = pppu::Config::FXP_LOG_TAYLOR;
    conf.fxp_sqrt_iters = 20;
    conf.fxp_sqrt_mode = pppu::Config::FXP_SQRT_GOLDSCHMIDT;
    conf.fxp_sin_iters = 20;
    conf.fxp_sin_mode = pppu::Config::FXP_SIN_TAYLOR;
    conf.fxp_tan_iters = 20;
    conf.fxp_tan_mode = pppu::Config::FXP_TAN_TAYLOR;

    return conf;
}

// Step 2 : Make network::MultiPartyPlayer object to define the communication among players.
std::unique_ptr<network::MultiPartyPlayer> make_netio(playerid_t pid, std::size_t num_parties, std::string ssl_dir) {
    // Endpoints, based on port number and number of parties
    int base_port = 2345;
    int num_threads = 1; // This pid's player use how many threads
    std::vector<boost::asio::ip::tcp::endpoint> endpoints;
    for(int i = 0; i < num_parties; ++i) {
        endpoints.emplace_back(
            boost::asio::ip::address::from_string("127.0.0.1"),
            base_port+i
        );
    }

    // MultiPartyPlayer, if using not empty ssl address, use SecureMultiPartyPlayer, if not, use PlainMultiPartyPlayer
    std::unique_ptr<network::MultiPartyPlayer> netio;
    if(ssl_dir == std::string(""))
    {
        auto plain_player = std::make_unique<network::PlainMultiPartyPlayer>(pid, num_parties);
        plain_player->run(num_threads);
        plain_player->connect(endpoints);
        netio = std::move(plain_player);
    }
    else
    {
        auto secure_player = std::make_unique<network::SecureMultiPartyPlayer>(pid, num_parties);
        secure_player->setup_ssl_context(ssl_dir);
        secure_player->run(num_threads);
        secure_player->connect(endpoints);
        netio = std::move(secure_player);
    }

    return netio;
}

// Step 3 : Make mpc::Protocol and mpc::Preprocessing objects to define the protocol.
template <typename ProtocolType, typename ProtocolTriple>
std::tuple<std::unique_ptr<mpc::Protocol>, std::unique_ptr<mpc::Preprocessing>> make_protocol(network::MultiPartyPlayer* netio) {
    std::unique_ptr<mpc::Protocol> prot;
    std::unique_ptr<mpc::Preprocessing> prep;
    if(netio==nullptr)
        throw std::invalid_argument("must provide valid netio");
    prep = std::make_unique<ProtocolTriple>();
    prot = std::make_unique<ProtocolType>(netio, prep.get());

    return std::make_tuple(std::move(prot), std::move(prep));
}

// Step 4 : Finally, make pppu::Context object used in operations in pppu::Value.
std::shared_ptr<pppu::Context> make_context(pppu::Config config, std::unique_ptr<mpc::Protocol> prot, std::unique_ptr<mpc::Preprocessing> prep, std::unique_ptr<network::MultiPartyPlayer> netio) {
    return std::make_shared<pppu::Context>(
        std::move(config),
        std::move(prot),
        std::move(prep),
        std::move(netio)
    );
}

// Step 5 : Example to process step 1 to 4.
std::shared_ptr<pppu::Context> run_player(std::size_t pid, std::size_t num_parties) {
    // Use TCP socket
    std::string ssl_dir = "";

    // Define parameters and objects
    int64_t fxp_fracbits = 40;
    int64_t fxp_security_parameter = 3;
    std::shared_ptr<pppu::Context> context;
    pppu::Config conf;
    std::unique_ptr<mpc::Protocol> prot;
    std::unique_ptr<mpc::Preprocessing> prep;
    std::unique_ptr<network::MultiPartyPlayer> netio;

    // Step 1 to 4
    conf = make_config(fxp_security_parameter, fxp_fracbits);
    netio = make_netio(pid, num_parties, ssl_dir);
    std::tie(prot, prep) = make_protocol<mpc::Semi2k, mpc::Semi2kTriple>(netio.get());
    context = make_context(conf, std::move(prot), std::move(prep), std::move(netio));

    return context;
}

/// @param ctx is the final version of context we use.
void tutorial_1_prepare(int pid, int num_parties) {
    // 2 player
    auto context = run_player(pid, num_parties);
    auto ctx = context.get();
}

}
