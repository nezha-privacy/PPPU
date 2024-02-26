#include "mp_connect.hpp"

namespace network {
namespace detail {

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
    bool order)
{

    using boost::asio::buffer;
    using boost::asio::use_awaitable;

    if (my_pid == peer_pid)
        throw std::runtime_error("self connection is unexpected");

    playerid_t recved_peer_pid;

    if (order)
    {
        co_await async_read(
            socket_recv, buffer(&recved_peer_pid, sizeof(recved_peer_pid)),
            use_awaitable);
        co_await async_write(
            socket_send, buffer(&my_pid, sizeof(my_pid)),
            use_awaitable);
    }
    else
    {
        co_await async_write(
            socket_send, buffer(&my_pid, sizeof(my_pid)),
            use_awaitable);
        co_await async_read(
            socket_recv, buffer(&recved_peer_pid, sizeof(recved_peer_pid)),
            use_awaitable);
    }

    bool is_successful = (recved_peer_pid == peer_pid);
    co_return is_successful;
};

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
    bool order)
{
    using boost::asio::use_awaitable;
    using boost::asio::ssl::stream_base::client;
    using boost::asio::ssl::stream_base::server;

    std::string peer_hostname = "Party" + std::to_string(peer_pid);
    socket_send.set_verify_mode(boost::asio::ssl::verify_peer);
    socket_recv.set_verify_mode(boost::asio::ssl::verify_peer);
    socket_send.set_verify_callback(boost::asio::ssl::host_name_verification(peer_hostname));
    socket_recv.set_verify_callback(boost::asio::ssl::host_name_verification(peer_hostname));

    if (order)
    {
        co_await socket_recv.async_handshake(server, use_awaitable);
        co_await socket_send.async_handshake(client, use_awaitable);
    }
    else
    {
        co_await socket_send.async_handshake(client, use_awaitable);
        co_await socket_recv.async_handshake(server, use_awaitable);
    }

    co_return true;
}

} // namespace detail

} // namespace network
