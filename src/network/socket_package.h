#pragma once

#include <vector>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace network
{

using TCPSocket = boost::asio::ip::tcp::socket;
using SSLSocket = boost::asio::ssl::stream<TCPSocket>;

/// @class SocketPackage
/// @brief A class stored send and receive sockets used in connections.
template <typename socket_type>
class SocketPackage
{

  protected:
    std::vector<socket_type> _sockets_send;
    std::vector<socket_type> _sockets_recv;

  public:
    using size_type = std::size_t;

    SocketPackage()                                 = default;
    ~SocketPackage()                                = default;
    SocketPackage(SocketPackage &&)                 = default;
    SocketPackage(const SocketPackage &)            = delete;
    SocketPackage &operator=(SocketPackage &&)      = default;
    SocketPackage &operator=(const SocketPackage &) = delete;

    /// @brief The constructor of SocketPackage.
    /// @param n The number of players
    /// @param args The args of SocketPackage
    template <typename... Args>
    SocketPackage(size_type n, Args &&...args)
    {
        init(n, std::forward<Args>(args)...);
    }

    /// @brief The initialization method of SocketPackage.
    /// @param n The number of players
    /// @param args The args of SocketPackage
    template <typename... Args>
    void init(size_type n, Args &&...args)
    {
        _sockets_send.clear();
        _sockets_recv.clear();
        for (size_type i = 0; i < n; ++i) {
            _sockets_send.emplace_back(std::forward<Args>(args)...);
            _sockets_recv.emplace_back(std::forward<Args>(args)...);
        }
    }

    /// @brief The constructor of SocketPackage, directly pass in sockets.
    /// @param sockets_send The passed in sockets used to send
    /// @param sockets_recv The passed in sockets used to receive
    SocketPackage(
        std::vector<socket_type> &&sockets_send,
        std::vector<socket_type> &&sockets_recv)
        : _sockets_send(std::move(sockets_send)),
          _sockets_recv(std::move(sockets_recv))
    {
        auto n_send = _sockets_send.size();
        auto n_recv = _sockets_recv.size();

        if (n_send != n_recv)
            throw std::invalid_argument("number of sockets mismatch");

        if (n_send <= 1)
            throw std::invalid_argument("too few sockets");
    }

    /// @brief Get the number of the sockets.
    /// @return The number of the sockets
    size_type size() const { return _sockets_send.size(); }

    /// @brief Get the i-th socket used to send.
    /// @param i The index of the socket
    /// @return The i-th socket used to send
    socket_type &send(size_type i) { return _sockets_send.at(i); }

    /// @brief Get the i-th socket used to receive.
    /// @param i The index of the socket
    /// @return The i-th socket used to receive
    socket_type &recv(size_type i) { return _sockets_recv.at(i); }
};

} // namespace network
