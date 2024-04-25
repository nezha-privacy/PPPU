#pragma once

#include "mp_connect.hpp"

#include "comm_package.hpp"
#include "multi_party_player.h"

namespace network
{

/************************ basic tool ************************/

/// @brief Insert empty byte vector at my pid.
/// @param vec Messages received
/// @param slot My player id
template <typename T>
requires std::is_default_constructible_v<T> && std::movable<T>
void insert_empty(std::vector<T> &vec, playerid_t slot)
{
    vec.emplace(vec.begin() + slot, T{});
}

/// @brief Insert empty byte vector at input mpid.
/// @param vec Messages received
/// @param slot Input mpid
template <typename T>
requires std::is_default_constructible_v<T> && std::movable<T>
void insert_empty(std::vector<T> &vec, mplayerid_t slots)
{
    for (auto slot : slots) {
        vec.emplace(vec.begin() + slot, T{});
    }
}

/// @brief Used in run function.
/// @param ioc IO context
/// @param n_threads Number of players' thread
/// @return Players' thread
std::vector<std::thread> run_io_context(boost::asio::io_context &ioc, std::size_t n_threads);

/// @brief Used in stop function.
/// @param ioc IO context
/// @param worker_threads Players' thread
void stop_io_context(boost::asio::io_context &ioc, std::vector<std::thread> &worker_threads);

/************************ base player ************************/

/************************ socket player ************************/

namespace detail
{

/// @brief Constructor
/// @param my_pid Player's id used to construct
/// @param n_players Number of players
template <typename SocketType>
SocketMultiPartyPlayer<SocketType>::SocketMultiPartyPlayer(
    playerid_t my_pid,
    size_type n_players)
    : MultiPartyPlayer(my_pid, n_players),
      _is_running(false),
      _work_guard(boost::asio::make_work_guard(_ioc))
{
}

/// @brief Deconstructor
template <typename SocketType>
SocketMultiPartyPlayer<SocketType>::~SocketMultiPartyPlayer()
{
    this->stop();
}

/// @brief Emulate different network conditions - set specific delay.
/// @param tos The mpid of players who are restricted by the function
/// @param delay Specific delay
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::set_delay(mplayerid_t tos, DurationType delay)
{
    _comm.set_delay(tos, delay);
}

/// @brief Emulate different network conditions - set specific bucket size.
/// @param tos The mpid of players who are restricted by the function
/// @param bitrate Specific bit rate
/// @param capacity Specific buffer capacity
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::set_bucket(
    mplayerid_t tos,
    BitrateType rate,
    size_type capacity)
{
    _comm.set_bucket(tos, rate, capacity);
}

/// @brief Get network statistics.
/// @return The network statistics such as traffic statistics
template <typename SocketType>
Statistics SocketMultiPartyPlayer<SocketType>::get_statistics() const
{
    auto stat = _comm.get_statistics();
    stat.elapsed_total = MultiPartyPlayer::_timer.total_elapsed();
    return stat;
}

/// @brief Judge whether the threads are running.
/// @return If they are running, return true, otherwise return false
template <typename SocketType>
bool SocketMultiPartyPlayer<SocketType>::is_running() const
{
    if (_is_running && _ioc.stopped()) {
        throw std::runtime_error("io context stopped unexpectedly");
    }
    return _is_running;
}

/// @brief Run all of the players' threads.
/// @param n_threads The number of players' thread.
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::run(size_type n_threads)
{
    if (this->is_running())
        throw std::runtime_error("player already running");

    assert(_worker_threads.size() == 0);
    _worker_threads = run_io_context(_ioc, n_threads);
    _is_running = true;
}

/// @brief Stop the running threads.
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::stop()
{
    stop_io_context(_ioc, _worker_threads);
    _is_running = false;
}

/// @brief Interconnect the endpoints together.
/// @param endpoints Endpoint of the participating players
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::connect(EndpointVector const &endpoints)
{
    using namespace std::chrono_literals;

    SocketPackageType sockets = this->get_empty_sockets();
    auto future = detail::mp_connect(_my_pid, _n_players, _ioc, sockets, endpoints);

    get_or_throw(future, 5s, "connect timeout");
    _comm = std::move(CommPackageType(std::move(sockets)));
}

/// @brief Implementation of clearing my buffer and sync with all other players.
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::impl_sync()
{
    ByteVector VERIFY_CODE { std::byte(0x31), std::byte(0x28), std::byte(0xaf), std::byte(0x9b) };
    auto msgs_recv = this->impl_broadcast_recv(VERIFY_CODE.copy());
    for(auto peer: all_but_me()) {
        if( msgs_recv[peer] != VERIFY_CODE ) {
            throw std::runtime_error("network synchronization error");
        }
    }
}

/// @brief Implementation of sending message to another player.
/// @param to Another receiver player's pid
/// @param message The message to be sent
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::impl_send(playerid_t to, ByteVector &&message)
{
    using namespace std::chrono_literals;
    auto message_send = std::move(message);

    auto future = _comm.send_copy(to, message_send);
    // auto etc = estimated_time_of_completion(Bytes(message.size()), _comm.get_bitrate(to), _comm.get_delay(to));
    // get_or_throw(future, etc + 1s, "send timeout");
    future.get();
}

/// @brief Implementation of receiving message from another player.
/// @param from Another sender player's pid
/// @param size_hint Estimation of the size of received information
/// @return The message to be received
template <typename SocketType>
ByteVector SocketMultiPartyPlayer<SocketType>::impl_recv(playerid_t from, size_type size_hint)
{
    using namespace std::chrono_literals;
    auto future = _comm.recv(from, size_hint);
    // return get_or_throw(future, 5s, "recv timeout");
    return future.get();
}

/// @brief Implementation of sending message to, then receiving from another player.
/// @param pid Another player's pid
/// @param message The message to be sent
/// @return The message to be received
template <typename SocketType>
ByteVector SocketMultiPartyPlayer<SocketType>::impl_exchange(playerid_t peer, ByteVector &&message)
{
    using namespace std::chrono_literals;
    auto message_send = std::move(message);

    auto size_hint = message_send.size();

    auto future_send = _comm.send_copy(peer, message_send);
    auto future_recv = _comm.recv(peer, size_hint);

    future_send.get();
    return future_recv.get();
}

/// @brief Implementation of sending a message to the next player, and receiving from the previous player. 
/// @param offset Number of intervals between participants
/// @param message The message to be sent
/// @return The message to be received
template <typename SocketType>
ByteVector SocketMultiPartyPlayer<SocketType>::impl_pass_around(offset_type offset, ByteVector &&message)
{
    using namespace std::chrono_literals;
    auto message_send = std::move(message);
    playerid_t to = _my_pid + offset;
    playerid_t from = _my_pid - offset;

    auto size_hint = message_send.size();

    auto future_send = _comm.send_copy(to, message_send);
    auto future_recv = _comm.recv(from, size_hint);

    future_send.get();
    return future_recv.get();
}

/// @brief Implementation of broadcasting message, then receive from all other players. 
/// @param message The message to be sent
/// @return The message to be received
template <typename SocketType>
mByteVector SocketMultiPartyPlayer<SocketType>::impl_broadcast_recv(ByteVector &&message)
{
    using namespace std::chrono_literals;
    auto message_send = std::move(message);

    auto size_hint = message_send.size();

    FutureVector<void> futures_send;
    FutureVector<ByteVector> futures_recv;
    for (auto peer : all_but_me()) {
        futures_send.emplace_back(_comm.send_copy(peer, message_send));
        futures_recv.emplace_back(_comm.recv(peer, size_hint));
    }

    futures_send.get();
    auto messages_recv = futures_recv.get();

    // insert empty byte vector at my pid
    insert_empty(messages_recv, _my_pid);
    return messages_recv;
}

/// @brief Implementation of broadcasting message to all the other players.
/// @param message The message to be broadcasted
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::impl_broadcast(ByteVector &&message)
{
    using namespace std::chrono_literals;
    auto message_send = std::move(message);

    FutureVector<void> futures_send;
    for (auto to : all_but_me()) {
        futures_send.emplace_back(_comm.send_copy(to, message_send));
    }

    futures_send.get();
}

/// @brief Implementation of sending different messages to other players separately.
/// @param to Other receiver players' mpid
/// @param message The message to be sent
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::impl_msend(mplayerid_t tos, mByteVector &&messages)
{
    using namespace std::chrono_literals;
    auto messages_send = std::move(messages);

    FutureVector<void> futures_send;
    for (auto to : tos) {
        futures_send.emplace_back(_comm.send_copy(to, messages_send.at(to)));
    }

    futures_send.get();
}

/// @brief Implementation of receiving different messages from other players separately.
/// @param from Other sender players' mpid
/// @param size_hint Estimation of the size of received information
/// @return The message to be received
template <typename SocketType>
mByteVector SocketMultiPartyPlayer<SocketType>::impl_mrecv(mplayerid_t froms, size_type size_hint)
{
    FutureVector<ByteVector> futures_recv;
    for (auto from : froms) {
        futures_recv.emplace_back(_comm.recv(from, size_hint));
    }
    auto messages_recv = futures_recv.get();

    // insert empty slot
    insert_empty(messages_recv, all() - froms);
    return messages_recv;
}

/// @brief Implementation of broadcasting the same message to a group of players using TCP socket.
/// @param message The message to be broadcasted
template <typename SocketType>
void SocketMultiPartyPlayer<SocketType>::impl_mbroadcast(mplayerid_t tos, ByteVector &&message)
{
    auto message_send = std::move(message); // memfree on destruction

    FutureVector<void> futures_send;

    for (auto to : tos) {
        futures_send.emplace_back(_comm.send_copy(to, message_send));
    }
    futures_send.get();
}

/// @brief Implementation of broadcasting the same message to a group of player.
/// @param group A group of player
/// @param message The message to be broadcasted
template <typename SocketType>
mByteVector SocketMultiPartyPlayer<SocketType>::impl_mbroadcast_recv(mplayerid_t group, ByteVector &&message)
{
    auto message_send = std::move(message);
    auto size_hint = message_send.size();
    FutureVector<void> futures_send;
    FutureVector<ByteVector> futures_recv;
    for (auto peer : group) {
        futures_send.emplace_back(_comm.send_copy(peer, message_send));
        futures_recv.emplace_back(_comm.recv(peer, size_hint));
    }
    futures_send.get();
    auto messages_recv = futures_recv.get();
    insert_empty(messages_recv, all() - group);
    return messages_recv;
}

} // namespace detail

/************************ secure player ************************/

/************************ plain player ************************/

/************************ local player ************************/

} // namespace network
