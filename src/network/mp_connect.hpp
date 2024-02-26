#pragma once

#include <iostream>
#include <map>
#include <future>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>

#include "futures.h"
#include "mp_connect.h"

namespace network {
namespace detail {

static auto &get_tcp_socket(TCPSocket &s) { return s; }
static auto &get_tcp_socket(SSLSocket &s) { return s.lowest_layer(); }

/// @brief Connect one endpoint with its acceptor.
/// @details { co_await socket.async_connect(...); } does not fail, but blocks when connection is refused by remote, 
///          connect socket_send to remote endpoint then accept socket_recv from remote.
/// @param socket_send The tcp socket used to send, this time is used to send connection information
/// @param socket_recv The tcp socket used to receive, this time is used to receive connection information
/// @param endpoint The sender endpoint of connection information
/// @param acceptor The receiver acceptor of connection information
/// @param order Use my_pid < peer_pid, the lower id connect first
template <typename SocketType>
boost::asio::awaitable<void> co_connect(
    SocketType &socket_send,
    SocketType &socket_recv,
    boost::asio::ip::tcp::endpoint const &endpoint,
    boost::asio::ip::tcp::acceptor &acceptor,
    bool order)
{
    using boost::asio::use_awaitable;

    auto &tcp_socket_send = get_tcp_socket(socket_send);
    auto &tcp_socket_recv = get_tcp_socket(socket_recv);

    if (order)
    {
        co_await acceptor.async_accept(tcp_socket_recv, use_awaitable);
        co_await tcp_socket_send.async_connect(endpoint, use_awaitable);
    }
    else
    {
        co_await tcp_socket_send.async_connect(endpoint, use_awaitable);
        co_await acceptor.async_accept(tcp_socket_recv, use_awaitable);
    }
}

/// @brief Handshake for plaintext socket.
/// @param my_pid The sender's pid
/// @param peer_pid The receiver's pid
/// @param socket_send The socket used to send connection information
/// @param socket_recv The socket used to receive connection information
/// @param Order Use my_pid < peer_pid, the lower id connect first
boost::asio::awaitable<bool> co_handshake(
    playerid_t my_pid,
    playerid_t peer_pid,
    TCPSocket &socket_send,
    TCPSocket &socket_recv,
    bool order);

/// @brief Handshake for ssl socket.
/// @param my_pid The sender's pid
/// @param peer_pid The receiver's pid
/// @param socket_send The socket used to send connection information
/// @param socket_recv The socket used to receive connection information
/// @param Order Use my_pid < peer_pid, the lower id connect first
boost::asio::awaitable<bool> co_handshake(
    playerid_t my_pid,
    playerid_t peer_pid,
    SSLSocket &socket_send,
    SSLSocket &socket_recv,
    bool order);

/// @brief The implementation of connecting to n-1 other players.
/// @param my_pid The one wants to connect with other n-1 players
/// @param n_players The number of other players
/// @param ioc Used to input and output for an endpoint
/// @param sockets The sockets used in connections
/// @param endpoints The endpoints which will connect together
template <typename SocketType>
boost::asio::awaitable<void> co_mp_connect(
    playerid_t my_pid,
    std::size_t n_players,
    boost::asio::io_context &ioc,
    SocketPackage<SocketType> &sockets,
    std::vector<boost::asio::ip::tcp::endpoint> const &endpoints)
{

    using boost::asio::use_awaitable;
    using boost::asio::ip::tcp;

    tcp::acceptor acceptor(ioc, endpoints.at(my_pid));

    // connect to server, then wait for connection from server
    for (playerid_t peer_pid = 0; peer_pid < n_players; ++peer_pid)
    {

        auto &endpoint = endpoints.at(peer_pid);
        auto &socket_send = sockets.send(peer_pid);
        auto &socket_recv = sockets.recv(peer_pid);

        if (my_pid != peer_pid)
        {
            co_await co_connect(socket_send, socket_recv, endpoint, acceptor, my_pid < peer_pid);
            co_await co_handshake(my_pid, peer_pid, socket_send, socket_recv, my_pid < peer_pid);
        }
    }
}

/// @brief Connect to n-1 other players.
/// @details Endpoints [   my_pid  ] : listen endpoint, 
///          endpoints [ other_pid ] : connect endpoint, 
///          mp_connect must be called in strictly increasing order of my_pid, 
///          that is, mp_connect(i, ...) happens before mp_connect(i+1, ...)
///          otherwise it would fail
/// @param my_pid The one wants to connect with other n-1 players
/// @param n_players The number of other players
/// @param ioc Used to input and output for an endpoint
/// @param sockets The sockets used in connections
/// @param endpoints The endpoints which will connect together
/// @param timeout The time limit of the connections
template <typename SocketType>
std::future<void> mp_connect(
    playerid_t my_pid,
    std::size_t n_players,
    boost::asio::io_context &ioc,
    SocketPackage<SocketType> &sockets,
    std::vector<boost::asio::ip::tcp::endpoint> const &endpoints)
{

    /************************ arguments check  ************************/

    if (ioc.stopped())
        throw std::runtime_error("io context stopped");
    if (my_pid >= n_players)
        throw std::invalid_argument("invalid pid");
    if (n_players != endpoints.size())
        throw std::invalid_argument("argument mismatch");
    if (n_players > mplayerid_t::MAX_NUM_PLAYERS)
        throw std::invalid_argument("too much players");

    /************************ launch async operations ************************/

    std::promise<void> promise;
    auto future = promise.get_future();

    co_spawn(
        ioc,
        co_mp_connect(my_pid, n_players, ioc, sockets, endpoints),
        [promise = std::move(promise)]
        (std::exception_ptr e) mutable
        {
            if (e)
                promise.set_exception(e);
            else
                promise.set_value();
        });

    return future;

}

} // namespace detail

} // namespace network
