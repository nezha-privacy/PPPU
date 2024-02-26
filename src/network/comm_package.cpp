#include "comm_package.hpp"

namespace network
{

namespace detail
{

/// @brief Implementation of communication function - set delay. 
/// @param delay Specific delay
/// @return The return type of a coroutine or asynchronous operation
boost::asio::awaitable<void> co_delay(std::chrono::steady_clock::duration delay)
{
    if (delay == decltype(delay)::zero())
        co_return;

    auto executor = co_await boost::asio::this_coro::executor;
    boost::asio::steady_timer timer(executor);
    timer.expires_after(delay);
    co_await timer.async_wait(boost::asio::use_awaitable);
}

/************************ token bucket ************************/

/// @brief Set the bit rate and capacity of the token bucket.
/// @param rate Specific bit rate
/// @param capacity Specific buffer capacity
void TokenBucket::set(BitrateType rate, size_type capacity)
{
    _rate = rate;
    _capacity = capacity;
    _available = capacity;
    _latest_update = Clock::now();
}

/// @brief Get the bit rate of the token bucket.
/// @return The bit rate of the token bucket
TokenBucket::BitrateType TokenBucket::bitrate() const
{
    return _rate;
}

/// @brief Get the capacity of the token bucket.
/// @return The capacity of the token bucket
TokenBucket::size_type TokenBucket::capacity() const
{
    return _capacity;
}

/// @brief No blocking, return immediately.
/// @param requested Number of tokens required
/// @return Number of tokens obtained
TokenBucket::size_type TokenBucket::request(size_type requested)
{
    auto now = Clock::now();
    auto elapsed = now - _latest_update;
    size_type newly_generated = Bytes(elapsed * _rate).count();
    _available = std::min(_capacity, _available + newly_generated);
    auto acquired = std::min(_available, requested);
    
    _available -= acquired;
    _latest_update = now;

    return acquired;
}

/// @brief Wait until the required number of tokens is reached.
/// @param required Number of tokens required
/// @return Awaitable object
boost::asio::awaitable<void> TokenBucket::require(size_type required)
{
    if( _rate == decltype(_rate)::zero() ) {
        throw std::runtime_error("token bucket bitrate set to zero");
    }

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using boost::asio::use_awaitable;
    using boost::asio::steady_timer;
    namespace this_coro = boost::asio::this_coro;

    // if acquired enough tokens, return immediately
    auto now = Clock::now();
    auto elapsed = (now - _latest_update);
    size_type newly_generated = Bytes(elapsed * _rate).count();
    _available = std::min(_capacity, _available + newly_generated);
    if(required <= _available)
    {
        // printf("acquire %lf MB immediately\n", required / 1e6);
        _available -= required;
        _latest_update = now;
        co_return;
    }
    else
    {
        // didn't acquire enough tokens, so wait for a while
        auto remaining = required - _available;
        auto ETA = duration_cast<DurationType>(Bytes(remaining) / _rate);
        auto completion_timepoint = now + ETA;
        auto executor = co_await this_coro::executor;
        steady_timer timer(executor);
        timer.expires_at(completion_timepoint);
        co_await timer.async_wait(use_awaitable);

        // printf("acquire %lf MB, complete after last update : %ld ms\n", required / 1e6, duration_cast<milliseconds>(completion_timepoint - _latest_update).count() );

        // token bucket should be empty now, so update timepoint only
        _available = 0;
        _latest_update = completion_timepoint;
    }
}

} // namespace detail

} // namespace network
