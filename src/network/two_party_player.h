#pragma once

#include "multi_party_player.hpp"

namespace network {

/// @class TwoPartyPlayer
/// @brief Player entity under two party communication conditions (based on multi player version).
class TwoPartyPlayer
{
public:
    using size_type = std::size_t;

    TwoPartyPlayer() = default;
    virtual ~TwoPartyPlayer() = default;

    virtual playerid_t id()     const = 0;
    virtual playerid_t peerid() const = 0;

    /// @brief Send message to the other player. Blocks until operation completes.
    virtual void send(ByteVector &&message) = 0;

    /// @brief Recv message from the other player. Length of message received is determined by the sender. Blocks until operation completes.
    virtual ByteVector recv() = 0;

    /// @brief Send message to, then recv message from the other player. Length of message received is determined by the sender. Blocks until operation completes.
    virtual ByteVector exchange(ByteVector &&message) = 0;
};

/// @class SecureTwoPartyPlayer
/// @brief TwoPartyPlayer that uses SSL Socket as its socket.
class SecureTwoPartyPlayer : public TwoPartyPlayer
{
protected:
    SecureMultiPartyPlayer _mplayer;
public:
    // A pair consisting of IP address and port number.
    using Endpoint = boost::asio::ip::tcp::endpoint;

    /// @brief Constructor.
    /// @param my_pid Player's id used to construct
    SecureTwoPartyPlayer(playerid_t my_pid): _mplayer(my_pid, 2) {}
    ~SecureTwoPartyPlayer() = default;

    /// @brief Return my pid.
    /// @return My pid
    playerid_t id()     const { return     _mplayer.id(); }

    /// @brief Return my partner's pid.
    /// @return My partner's pid
    playerid_t peerid() const { return 1 - _mplayer.id(); }

    /// @brief Used only in SecureTwoPartyPlayer to set up SSL protocol's context.
    /// @param ssl_dir SSL protocol's verify path
    void setup_ssl_context(std::string const& ssl_dir) { _mplayer.setup_ssl_context(ssl_dir); }

    /// @brief Run both of the players' threads.
    /// @param n_threads The number of players' thread.
    void run(size_type n_threads) { _mplayer.run(n_threads); }

    /// @brief Stop the running threads.
    void stop() { _mplayer.stop(); }

    /// @brief Judge whether the threads are running.
    /// @return If they are running, return true, otherwise return false
    bool is_running() const { return _mplayer.is_running(); }

    /// @brief Interconnect the endpoints together.
    /// @param me Endpoint of the current thread player.
    /// @param peer Endpoint of the current thread player's partner.
    void connect(Endpoint me, Endpoint peer) {
        _mplayer.connect( {id() ? peer : me, id() ? me : peer } );
    }

    /// @brief Send message to my partner.
    /// @note Blocks until operation completes.
    /// @param message The message to be sent
    void send(ByteVector&& message) { _mplayer.send(peerid(), std::move(message)); }

    /// @brief Receive message from my partner.
    /// @note Length of message received is determined by the sender. Blocks until operation completes.
    /// @return The message to be received
    ByteVector recv() { return _mplayer.recv(peerid()); }

    /// @brief Exchange messages with my partner.
    /// @note Blocks until operation completes.
    /// @param message The message to be sent by myself
    /// @return The message to be received by myslef
    ByteVector exchange(ByteVector&& message) { return _mplayer.exchange(peerid(), std::move(message)); }
};

/// @class PlainTwoPartyPlayer
/// @brief TwoPartyPlayer that uses TCP Socket as its socket.
class PlainTwoPartyPlayer : public TwoPartyPlayer
{
protected:
    PlainMultiPartyPlayer _mplayer;
public:
    // A pair consisting of IP address and port number.
    using Endpoint = boost::asio::ip::tcp::endpoint;

    /// @brief Constructor
    /// @param my_pid Player's id used to construct
    PlainTwoPartyPlayer(playerid_t my_pid): _mplayer(my_pid, 2) {}
    ~PlainTwoPartyPlayer() = default;

    /// @brief Return player's id.
    /// @return Player's id.
    playerid_t id()     const { return     _mplayer.id(); }

    /// @brief Return player's partner's id
    /// @return Player's partner's id
    playerid_t peerid() const { return 1 - _mplayer.id(); }

    /// @brief Run both of the players' threads.
    /// @param n_threads The number of players' thread.
    void run(size_type n_threads) { _mplayer.run(n_threads); }

    /// @brief Stop the running threads.
    void stop() { _mplayer.stop(); }

    /// @brief Judge whether the threads are running.
    /// @return If they are running, return true, otherwise return false
    bool is_running() const { return _mplayer.is_running(); }

    /// @brief Interconnect the endpoints together.
    /// @param me Endpoint of the current thread player.
    /// @param peer Endpoint of the current thread player's partner.
    void connect(Endpoint me, Endpoint peer) {
        _mplayer.connect( {id() ? peer : me, id() ? me : peer } );
    }

    /// @brief Send message to my partner.
    /// @param message The message to be sent
    void send(ByteVector&& message) { _mplayer.send(peerid(), std::move(message)); }

    /// @brief Receive message from my partner.
    /// @return The message to be received
    ByteVector recv() { return _mplayer.recv(peerid()); }

    /// @brief Exchange messages with my partner.
    /// @param message The message to be sent by myself
    /// @return The message to be received by myslef
    ByteVector exchange(ByteVector&& message) { return _mplayer.exchange(peerid(), std::move(message)); }

};

} // namespace network
