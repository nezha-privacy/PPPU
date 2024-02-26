#pragma once

#include <future>
#include <algorithm>
#include <variant>
#include <cstdio>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/steady_timer.hpp>

#include "statistics.h"
#include "bitrate.hpp"
#include "socket_package.h"

#include "../tools/byte_vector.h"
#include "../tools/timer.h"

namespace network {

namespace detail {

/// @class TokenBucket
/// @brief Used to limit traffic.
class TokenBucket {
public:
    using size_type = std::size_t;
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using DurationType = Clock::duration;
    using BitrateType = GigaBitsPerSecond;

protected:

    BitrateType  _rate;       // token generation rate
    size_type    _capacity;   // bucket capacity
    size_type    _available;  // current available tokens

    TimePoint    _latest_update;

public:

    ~TokenBucket() = default;
    TokenBucket(TokenBucket&&) = default;
    TokenBucket(TokenBucket const&) = delete;
    TokenBucket& operator=(TokenBucket&&) = default;
    TokenBucket& operator=(TokenBucket const&) = delete;

    /// @brief Default constructor, set limit to umlimited.
    TokenBucket() { set(BitrateType::unlimited(), 0); }

    /// @brief Set limit to certain input level.
    /// @param rate Specific bit rate
    /// @param capacity Specific buffer capacity
    TokenBucket(BitrateType rate, size_type capacity) { set(rate, capacity); }

    /// @brief Implementation of setting limit to certain input level.
    /// @param rate Specific bit rate
    /// @param capacity Specific buffer capacity
    void set(BitrateType rate, size_type capacity);

    /// @brief Get the bit rate limit.
    /// @return The bit rate limit
    BitrateType bitrate()  const;

    /// @brief Get the capacity limit.
    /// @return The capacity limit
    size_type capacity()   const;

    /// @brief No blocking, return immediately.
    /// @param requested Number of tokens required
    /// @return Number of tokens obtained
    size_type request(size_type requested);

    /// @brief Wait until the required number of tokens is reached.
    /// @param required Number of tokens required
    /// @return Awaitable object
    boost::asio::awaitable<void> require(std::size_t required);

};

/// @struct Strategy
/// @brief Strategies for traffic restrictions.
struct Strategy {
    enum class Type { unlimited, fixed_packet_size, fixed_interval, dynamic_packet_size };
    using DatasizeType = Bytes;
    using DurationType = std::chrono::steady_clock::duration;

    Type type;
    std::variant<DatasizeType, DurationType> data;

    /// @brief Default constructor, set the strategy to umlimited.
    Strategy(): type(Type::unlimited) {}

    /// @brief Constructor, set the strategy to fixed send interval limitation.
    /// @param interval Wait to send next message until next interval
    Strategy(DurationType interval): type(Type::fixed_interval), data(interval) {}

    /// @brief Constructor, set the strategy to fixed packet size limitation.
    /// @param packet_size Send a fixed size package
    Strategy(DatasizeType packet_size) : type(Type::fixed_packet_size), data(packet_size) {}

    /// @brief Constructor, set the strategy to dynamic packet size limitation.
    Strategy(Type type): type(type) { assert(type == Type::dynamic_packet_size); }
};

/// @class Sender
/// @brief Used to send message under certain limitation and strategy.
template <typename SocketType>
class Sender {
public:
    using size_type    = std::size_t;
    using DurationType = std::chrono::steady_clock::duration;
    using BitrateType  = TokenBucket::BitrateType;

protected:

    // statistics
    Timer _timer;
    size_type _bytes_send;

    // restrictions
    DurationType _delay;
    TokenBucket _bucket;
    Strategy _strategy;

    // socket
    SocketType _socket;

public:
    ~Sender() = default;
    Sender(Sender&&) = default;
    Sender(Sender const&) = delete;

    /// @brief Constructor, set the socket type.
    /// @param socket The socket type
    Sender(SocketType socket):
        _socket(std::move(socket)), _bytes_send(0), _delay(0) {}

    /// @brief Set the delay between every sending.
    /// @param delay Specific delay
    void set_delay (DurationType delay);

    /// @brief Set the bit rate and capacity of its token bucket.
    /// @param rate Specific bit rate
    /// @param capacity Specific buffer capacity
    void set_bucket(BitrateType rate, size_type capacity);

    /// @brief Get the delay between every sending.
    /// @return The delay between every sending
    DurationType get_delay()           const;

    /// @brief Get the bit rate of its token bucket.
    /// @return The bit rate of its token bucket
    BitrateType  get_bucket_bitrate()  const;

    /// @brief Get the capacity of its token bucket.
    /// @return The capacity of its token bucket
    size_type    get_bucket_capacity() const;

    /// @brief Get the number of byte the sender have sent.
    /// @return The number of byte the sender have sent.
    size_type    get_bytes_send()   const { return _bytes_send; }

    /// @brief Get the time the sender have used to send message.
    /// @return The time the sender have used to send message
    DurationType get_elapsed_send() const { return _timer.elapsed(); }

    /// @brief Copy the message to the sender used in future sending.
    /// @param message The message used to copy
    /// @return Future communiacation
    std::future<void> send_copy(ByteVector const& message);

};

/// @class Recver
/// @brief Used to receive message under certain limitation and strategy.
template <typename SocketType>
class Recver {
protected:
    using size_type    = std::size_t;
    using DurationType = std::chrono::steady_clock::duration;

    // statistics
    Timer _timer;
    size_type _bytes_recv;

    SocketType _socket;

public:
    ~Recver()             = default;
    Recver(Recver&&)      = default;
    Recver(Recver const&) = delete;

    /// @brief Constructor, set the socket type.
    /// @param socket The socket type
    Recver(SocketType socket): _bytes_recv(0), _socket(std::move(socket)) {}

    /// @brief Get the number of byte the receiver have received.
    /// @return The number of byte the receiver have received.
    size_type    get_bytes_recv()   const { return _bytes_recv; }

    /// @brief Get the time the receiver have used to receive message.
    /// @return The time the receiver have used to receive message
    DurationType get_elapsed_recv() const { return _timer.elapsed(); }

    std::future<ByteVector> recv(size_type size_hint);
};

/// @class CommPackage
/// @brief The main class used to send and receive messages. Contains senders and receivers.
template <typename SocketType>
class CommPackage {

protected:

    std::vector<Sender<SocketType>> _senders;
    std::vector<Recver<SocketType>> _recvers;

public:
    using size_type    = std::size_t;
    using DurationType = TokenBucket::DurationType;
    using BitrateType  = TokenBucket::BitrateType;

    CommPackage() = default;
    ~CommPackage() = default;
    CommPackage(CommPackage&&) = default;
    CommPackage(CommPackage const&) = delete;
    CommPackage& operator=(CommPackage&&) = default;
    CommPackage& operator=(CommPackage const&) = delete;

    /// @brief Constructor, set the socket type for senders and receivers.
    /// @param socket The socket type
    CommPackage(SocketPackage<SocketType> sockets) {
        size_type n_players = sockets.size();
        for(size_type i = 0; i < n_players; ++i) {
            _senders.emplace_back( std::move(sockets.send(i)) );
            _recvers.emplace_back( std::move(sockets.recv(i)) );
        }
    }

    /// @brief Set the delay between every sending.
    /// @param tos The mpid of players who are restricted by the function
    /// @param delay Specific delay
    void set_delay(mplayerid_t tos, TokenBucket::DurationType delay) {
        for(auto i: tos)    _senders.at(i).set_delay(delay);
    }

    /// @brief Set the bit rate and capacity of its token bucket.
    /// @param tos The mpid of players who are restricted by the function
    /// @param rate Specific bit rate
    /// @param capacity Specific buffer capacity
    void set_bucket(mplayerid_t tos, BitrateType rate, size_type capacity) {
        for(auto i: tos)
            _senders.at(i).set_bucket(rate, capacity);
    }

    /// @brief Get the number of players.
    /// @return Number of players
    size_type get_n_players() const { return _senders.size(); }

    /// @brief Copy the message to the sender used in future sending.
    /// @param to The player the message is sent to
    /// @param message The message used to copy
    /// @return Future communiacation
    std::future<void> send_copy(playerid_t to, ByteVector const& message) {
        return _senders.at(to).send_copy(message);
    }

    /// @brief Receive the message to the receiver used in future sending.
    /// @param from The player the message is received from
    /// @param size_hint Estimation of the size of received information
    /// @return Future communiacation
    std::future<ByteVector> recv(playerid_t from, size_type size_hint) {
        return _recvers.at(from).recv(size_hint);
    }

    /// @brief Get network statistics.
    /// @return The network statistics such as traffic statistics
    Statistics get_statistics() const;

};

} // namespace detail

} // namespace network
