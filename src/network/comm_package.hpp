#pragma once

#include <algorithm>
#include <future>
#include <cstdio>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>

#include "bitrate.hpp"
#include "comm_package.h"

namespace network
{

/************************ helper functions ************************/
namespace detail
{

/// @brief Implementation of communication function - receive, using certain socket. 
/// @param socket The socket type
/// @param size_hint Estimation of the size of received information
/// @return The return type of a coroutine or asynchronous operation
template <typename SocketType>
boost::asio::awaitable<ByteVector> co_recv(
    SocketType &socket,
    std::size_t size_hint)
{
    using boost::asio::async_read;
    using boost::asio::buffer;
    using boost::asio::use_awaitable;

    ByteVector message;
    ByteVector::size_type msg_size;

    // debug
    // using std::chrono::duration_cast;
    // using std::chrono::milliseconds;
    // using std::chrono::steady_clock;
    // Timer t; t.start();

    // message.reserve(size_hint);
    message.resize(size_hint);
    // printf("after reserve: size=%ld, capacity=%ld\n", message.size(), message.capacity());

    // debug
    // t.stop(); auto e0 = t.elapsed(); t.start();

    co_await async_read(
        socket,
        buffer(&msg_size, sizeof(msg_size)),
        use_awaitable);

    // debug
    // t.stop(); auto e1 = t.elapsed(); t.start();

    message.resize(msg_size);

    // debug
    // t.stop(); auto e2 = t.elapsed(); t.start();

    co_await async_read(
        socket,
        buffer(message.data(), msg_size),
        use_awaitable);

    // debug
    // t.stop(); auto e3 = t.elapsed();
    // printf("reserve in %ld ms, recv size in %ld ms, resize in %ld ms, recv message in %ld ms\n",
    //     duration_cast<milliseconds>(e0).count(),
    //     duration_cast<milliseconds>(e1).count(),
    //     duration_cast<milliseconds>(e2).count(),
    //     duration_cast<milliseconds>(e3).count()
    // );

    co_return message;
}

/// @brief Implementation of communication function - set delay. 
/// @param delay Specific delay
/// @return The return type of a coroutine or asynchronous operation
boost::asio::awaitable<void> co_delay(std::chrono::steady_clock::duration delay);

/// @brief Implementation of communication function - send messgae of a certain size. 
/// @return The return type of a coroutine or asynchronous operation
template <typename SocketType>
boost::asio::awaitable<void> co_send_size(
    SocketType &socket,
    std::size_t size)
{
    using boost::asio::async_write;
    using boost::asio::use_awaitable;

    co_await async_write(
        socket,
        boost::asio::buffer(&size, sizeof(size)),
        use_awaitable);
}

/// @brief Implementation of communication function - send buffer without speed limit. 
/// @return The return type of a coroutine or asynchronous operation
template <typename SocketType>
boost::asio::awaitable<void> co_send_buffer_unlimited(
    SocketType &socket,
    boost::asio::const_buffer buffer)
{
    using boost::asio::async_write;
    using boost::asio::use_awaitable;

    co_await async_write(socket, buffer, use_awaitable);
}

/// @brief Implementation of communication function - send buffer with speed limit by token bucket, send fixed packet size. 
/// @return The return type of a coroutine or asynchronous operation
template <typename SocketType>
boost::asio::awaitable<void> co_send_buffer_fixed_packet_size(
    SocketType &socket,
    boost::asio::const_buffer buf,
    TokenBucket &bucket,
    Bytes fixed_packet_size)
{
    using boost::asio::async_write;
    using boost::asio::use_awaitable;

    unsigned long long sent = 0;
    while (sent < buf.size()) {
        auto remaining = buf.size() - sent;
        auto this_packet_size = std::min(fixed_packet_size.count(), remaining);

        co_await bucket.require(this_packet_size);
        co_await async_write(socket,
                             boost::asio::buffer((char *)buf.data() + sent, this_packet_size),
                             use_awaitable);
        sent += this_packet_size;
    }
}

/// @brief Implementation of communication function - send buffer with speed limit by token bucket, send with fixed interval. 
/// @return The return type of a coroutine or asynchronous operation
template <typename SocketType>
boost::asio::awaitable<void> co_send_buffer_fixed_interval(
    SocketType &socket,
    boost::asio::const_buffer buffer,
    TokenBucket &bucket,
    std::chrono::steady_clock::duration fixed_interval)
{
    using boost::asio::async_write;
    using boost::asio::steady_timer;
    using boost::asio::use_awaitable;
    using std::chrono::steady_clock;
    auto executor = co_await boost::asio::this_coro::executor;

    steady_timer timer(executor);

    // record initiation time point
    auto completion_time_point = steady_clock::now();

    unsigned long long total_sent = 0;
    while (total_sent < buffer.size()) {
        // request tokens from bucket
        auto remaining = buffer.size() - total_sent;
        auto this_packet_size = bucket.request(remaining);

        // send packet
        co_await async_write(
            socket,
            boost::asio::buffer((char *)buffer.data() + total_sent, this_packet_size),
            use_awaitable);
        total_sent += this_packet_size;
        // std::cout << "total sent" << total_sent << std::endl;
        // std::cout << "remaining" << remaining << std::endl;

        // send complete, return immediately
        if (total_sent == buffer.size())
            break;

        // wait until next interval
        completion_time_point += fixed_interval;
        timer.expires_at(completion_time_point);
        co_await timer.async_wait(use_awaitable);
    }
}

/// @brief Implementation of communication function - send buffer with speed limit by token bucket, send dynamic packet size. 
/// @return The return type of a coroutine or asynchronous operation
// TODO: problem with this version:
// if you send very small packet very frequently,
// you will exceed specified bitrate limit
// because this function is stateless
// so maybe we should use token bucket?
template <typename SocketType>
boost::asio::awaitable<void> co_send_buffer_dynamic_packet_size(
    SocketType &socket,
    boost::asio::const_buffer buf,
    TokenBucket &bucket)
{
    auto bitrate = bucket.bitrate();

    // make sure the bitrate is valid
    if (bitrate == decltype(bitrate)::unlimited()) {
        throw std::invalid_argument("unlimited bitrate");
    }

    using boost::asio::async_write;
    using boost::asio::steady_timer;
    using boost::asio::use_awaitable;
    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;
    using std::chrono::milliseconds;
    using std::chrono::steady_clock;
    using namespace std::chrono_literals;
    using size_type = std::size_t;

    static constexpr auto min_duration = duration_cast<nanoseconds>(2ms);

    auto min_packet_size = Bytes(min_duration * bitrate).count();
    auto max_packet_size = bucket.capacity();

    // bucket capacity too small, resulting in frequent
    // context swtich and thus poor performance
    if(min_packet_size >= bucket.capacity())
        throw std::invalid_argument("bucket capacity too small");

    auto executor = co_await boost::asio::this_coro::executor;

    steady_timer timer(executor);
    auto completion_timepoint = steady_clock::now();

    // packet too small, send all at once and return immediately
    if (buf.size() < min_packet_size) {
        co_await bucket.require(buf.size());
        co_await async_write(socket, buf, use_awaitable);
        co_return;
    }

    // look for moderate initial packet size, in case of very large packet
    size_type initial_d = 1;
    while ((buf.size() >> initial_d) >= max_packet_size) {
        initial_d += 1;
    }

    // debug
    auto last_update = steady_clock::now();

    // send splitted packet
    for (size_type i = 0, sent = 0, d = initial_d;
         sent < buf.size(); ++i, ++d) {

        // stay on initial d for 2^initial_d - 1 times
        if (i < ((1 << initial_d) - 1)) {
            d = initial_d;
        }

        size_type packet_size = (buf.size() >> d);

        // packet too small, send all remaining and return immediately
        if (packet_size <= min_packet_size) {
            auto remaining = buf.size() - sent;
            packet_size = remaining;
        }

        // wait for enough tokens
        co_await bucket.require(packet_size);

        // send some data
        co_await async_write(
            socket,
            boost::asio::buffer((char *)buf.data() + sent, packet_size),
            use_awaitable);
        sent += packet_size;

    }
}

/// @brief Implementation of communication function - send using const lvalue reference for broadcasting. 
/// @return The return type of a coroutine or asynchronous operation
template <typename SocketType>
boost::asio::awaitable<void> co_send_byte_vector_copy(
    SocketType &socket,
    ByteVector const &message,
    std::chrono::steady_clock::duration delay,
    TokenBucket &bucket)
{
    using boost::asio::async_write;
    using boost::asio::use_awaitable;

    auto buffer = boost::asio::const_buffer(message.data(), message.size());

    co_await co_delay(delay);
    co_await co_send_size(socket, message.size());

    if ( bucket.bitrate() == decltype(bucket.bitrate())::unlimited() )
    {
        co_await co_send_buffer_unlimited(socket, buffer);
    }
    else
    {
        co_await co_send_buffer_dynamic_packet_size(socket, buffer, bucket);
    }

    // switch (strategy.type) {
    //     case Strategy::Type::unlimited: {
    //         co_await co_send_buffer_unlimited(socket, buffer);
    //         break;
    //     }
    //     case Strategy::Type::fixed_interval: {
    //         auto fixed_interval = std::get<Strategy::DurationType>(strategy.data);
    //         co_await co_send_buffer_fixed_interval(socket, buffer, bucket, fixed_interval);
    //         break;
    //     }
    //     case Strategy::Type::fixed_packet_size: {
    //         auto fixed_packet_size = std::get<Strategy::DatasizeType>(strategy.data);
    //         co_await co_send_buffer_fixed_packet_size(socket, buffer, bucket, fixed_packet_size);
    //         break;
    //     }
    //     case Strategy::Type::dynamic_packet_size: {
            // co_await co_send_buffer_dynamic_packet_size(socket, buffer, bucket);
    //         break;
    //     }
    // }
}


/************************  sender ************************/

/// @brief Set the delay between every sending.
/// @param delay Specific delay
template <typename SocketType>
void Sender<SocketType>::set_delay(TokenBucket::DurationType delay)
{
    _delay = delay;
}

/// @brief Set the bit rate and capacity of its token bucket.
/// @param rate Specific bit rate
/// @param capacity Specific buffer capacity
template <typename SocketType>
void Sender<SocketType>::set_bucket(BitrateType rate, size_type capacity)
{
    _bucket.set(rate, capacity);
}

/// @brief Get the delay between every sending.
/// @return The delay between every sending
template <typename SocketType>
Sender<SocketType>::DurationType Sender<SocketType>::get_delay() const
{
    return _delay;
}

/// @brief Get the bit rate of its token bucket.
/// @return The bit rate of its token bucket
template <typename SocketType>
Sender<SocketType>::BitrateType Sender<SocketType>::get_bucket_bitrate() const
{
    return _bucket.bitrate();
}

/// @brief Get the capacity of its token bucket.
/// @return The capacity of its token bucket
template <typename SocketType>
Sender<SocketType>::size_type Sender<SocketType>::get_bucket_capacity() const
{
    return _bucket.capacity();
}

/// @brief Copy the message to the sender used in future sending.
/// @param message The message used to copy
/// @return Future communiacation
template <typename SocketType>
std::future<void> Sender<SocketType>::send_copy(ByteVector const &message)
{
    using boost::asio::co_spawn;
    auto executor = _socket.get_executor();

    std::promise<void> promise_send;
    auto future_send = promise_send.get_future();

    auto task_send = detail::co_send_byte_vector_copy(
        _socket, message, _delay, _bucket);

    auto callback = [this, &message, promise_send = std::move(promise_send)](std::exception_ptr e) mutable {
        this->_timer.stop();
        if (e)
            promise_send.set_exception(e);
        else {
            this->_bytes_send += message.size();
            promise_send.set_value();
        }
    };

    _timer.start();
    co_spawn(
        executor, std::move(task_send), std::move(callback));
    return future_send;
}

/************************ recver ************************/

/// @brief Constructor, set the socket type.
/// @param socket The socket type
template <typename SocketType>
std::future<ByteVector> Recver<SocketType>::recv(size_type size_hint)
{
    using boost::asio::co_spawn;
    auto executor = _socket.get_executor();

    std::promise<ByteVector> promise_recv;
    auto future_recv = promise_recv.get_future();

    _timer.start();
    co_spawn(
        executor, detail::co_recv(_socket, size_hint),
        [this, promise_recv = std::move(promise_recv)](std::exception_ptr e, ByteVector message_recv) mutable {
            this->_timer.stop();
            if (e)
                promise_recv.set_exception(e);
            else {
                this->_bytes_recv += message_recv.size();
                promise_recv.set_value(std::move(message_recv));
            }
        });

    return future_recv;
}

/// @brief Get network statistics.
/// @return The network statistics such as traffic statistics
template <typename SocketType>
Statistics CommPackage<SocketType>::get_statistics() const
{
    size_type n_players = get_n_players();

    Statistics stat;
    stat.bytes_send.resize(n_players);
    stat.bytes_recv.resize(n_players);
    stat.elapsed_send.resize(n_players);
    stat.elapsed_recv.resize(n_players);

    for (size_type i = 0; i < n_players; ++i) {
        stat.bytes_send.at(i) = _senders.at(i).get_bytes_send();
        stat.bytes_recv.at(i) = _recvers.at(i).get_bytes_recv();

        stat.elapsed_send.at(i) = _senders.at(i).get_elapsed_send();
        stat.elapsed_recv.at(i) = _recvers.at(i).get_elapsed_recv();
    }

    return stat;
}

} // namespace detail

} // namespace network
