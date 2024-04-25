#pragma once

#include "example/utils.hpp"

using tcp = boost::asio::ip::tcp;
using address = boost::asio::ip::address;
using ProtocolType = mpc::Semi2k;
using ProtocolTriple = mpc::Semi2kTriple;

std::unique_ptr<network::MultiPartyPlayer> make_netio(
    playerid_t pid,
    std::size_t num_parties,
    std::string ssl_dir
) {
    int base_port = 7777;
    int num_threads = 1;

    std::vector<tcp::endpoint> endpoints;
    for(int i = 0; i < num_parties; ++i) {
        endpoints.emplace_back(
            address::from_string("127.0.0.1"),
            base_port+i
        );
    }

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

pppu::Config make_config(
    int64_t fxp_security_parameter,
    int64_t fxp_fracbits
) {
    pppu::Config conf;

    conf.fxp_fracbits = fxp_fracbits;
    conf.fxp_security_parameter = fxp_security_parameter;

    conf.fxp_div_iters = 20;
    conf.fxp_div_mode = pppu::Config::FXP_DIV_NEWTON;

    conf.fxp_sigmoid_iters = 50;
    conf.fxp_sigmoid_mode = pppu::Config::FXP_SIGMOID_DIV;

    conf.fxp_exp_iters = 20;
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

std::shared_ptr<pppu::Context> make_context(
    pppu::Config config,
    std::unique_ptr<mpc::Protocol> prot,
    std::unique_ptr<mpc::Preprocessing> prep,
    std::unique_ptr<network::MultiPartyPlayer> netio
) {
    return std::make_shared<pppu::Context>(
        std::move(config),
        std::move(prot),
        std::move(prep),
        std::move(netio)
    );
}

std::shared_ptr<pppu::Context> run_player(std::size_t pid, std::size_t num_parties) {
    std::string ssl_dir = "";
    int64_t fxp_fracbits = 40;
    int64_t fxp_security_parameter = 3;
    std::shared_ptr<pppu::Context> context;
    pppu::Config conf;
    std::unique_ptr<mpc::Protocol> prot;
    std::unique_ptr<mpc::Preprocessing> prep;
    std::unique_ptr<network::MultiPartyPlayer> netio;
    conf = make_config(fxp_security_parameter, fxp_fracbits);
    netio = make_netio(pid, num_parties, ssl_dir);
    std::tie(prot, prep) = make_protocol<ProtocolType, ProtocolTriple>(netio.get());
    context = make_context(conf, std::move(prot), std::move(prep), std::move(netio));
    return context;
}

int rand_32() {
    std::random_device rd;
    std::default_random_engine r_eng(rd());
    int randoxNumber = r_eng() % 10000000;
    int signal = r_eng() % 2;
    if(signal == 0) signal = 1;
    else signal = -1;
    return randoxNumber * signal;
}
float rand_f(){
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<float> dis(-32.0f, 32.0f);
    float randomFloat = dis(gen);
    return randomFloat;
}
