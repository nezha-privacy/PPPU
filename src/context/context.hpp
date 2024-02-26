#pragma once

#include <memory>

#include "network/multi_party_player.h"
#include "mpc/protocol.hpp"
#include "mpc/preprocessing.hpp"

namespace pppu
{

/// @struct Config
/// @brief Used to set parameters for fixed-point number calculation.
struct Config
{
    // probability of truncation error
    int64_t fxp_security_parameter;

    // number of max binary bits used for fraction part
    int64_t fxp_fracbits;

    // fixed: newton
    int64_t fxp_div_iters;
    enum { FXP_DIV_NEWTON } fxp_div_mode;

    // fixed: euler
    int64_t fxp_sigmoid_iters;
    enum { FXP_SIGMOID_EULER } fxp_sigmoid_mode;

    // fast: euler
    // high precision: taylor
    int64_t fxp_exp_iters;
    int64_t fxp_exp_intbits;
    enum { FXP_EXP_EULER, FXP_EXP_TAYLOR } fxp_exp_mode;

    // fixed: taylor
    int64_t fxp_log_iters;
    enum { FXP_LOG_TAYLOR } fxp_log_mode;

    // fixed: goldschmidt
    int64_t fxp_sqrt_iters;
    enum { FXP_SQRT_GOLDSCHMIDT } fxp_sqrt_mode;

    // fixed: taylor
    int64_t fxp_sin_iters;
    enum { FXP_SIN_TAYLOR } fxp_sin_mode;

    // fast: div
    // high precision: taylor
    int64_t fxp_tan_iters;
    enum { FXP_TAN_DIV, FXP_TAN_TAYLOR } fxp_tan_mode;

};

/// @class Context
/// @brief Used to set parameters for network communication and calculation.
class Context
{
private:
    std::unique_ptr<mpc::Protocol> _prot;
    std::unique_ptr<mpc::Preprocessing> _prep;
    std::unique_ptr<network::MultiPartyPlayer> _netio;

    Config _config;

public:

    /// @brief Constructor of Config.
    /// @param config Used fixed-point number calculation parameters
    /// @param prot Used encryption protocol
    /// @param prep Used preprocessing
    /// @param netio Used network communication settings
    Context(Config config,
        std::unique_ptr<mpc::Protocol> prot,
        std::unique_ptr<mpc::Preprocessing> prep,
        std::unique_ptr<network::MultiPartyPlayer> netio
    ): _prot( std::move(prot) ), _prep( std::move(prep) ), _netio( std::move(netio) ), _config( std::move(config) ) {}

    /// @brief Get the encryption protocol used in calculation.
    /// @return Encryption protocol used in calculation
    template <typename T>   T* prot()  const { return (T*)_prot.get(); }

    /// @brief Get the network communication settings.
    /// @return Network communication settings
    network::MultiPartyPlayer* netio() const { return _netio.get();    }
    // Used in calculate comm in benchmark (Please remove comments when using benchmark).
    network::PlainMultiPartyPlayer* netio_p() const { return _netio.get();    }
    network::SecureMultiPartyPlayer* netio_s() const { return _netio.get();    }

    /// @brief Get the fixed-point number calculation parameters.
    /// @return Fixed-point number calculation parameters
    Config* config() { return &_config; }

    /// @brief Get the number of max binary bits used for fraction part.
    /// @return Number of max binary bits used for fraction part
    int64_t fxp_fracbits() const { return _config.fxp_fracbits; }

    /// @brief Get the participant ID of a specific process.
    /// @return the participant ID of a specific process
    playerid_t  pid() const { return _netio->id(); }

    /// @brief Get the number of participants in communication.
    /// @return The number of participants in communication
    std::size_t num_parties() const { return _netio->num_players(); }

};

} // namespace pppu
