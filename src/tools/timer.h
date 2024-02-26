#pragma once

#include <chrono>

/// @class Timer
/// @brief A timer class, used in situations such as determining connection timeout.
class Timer {
public:
    using Clock         = std::chrono::steady_clock;
    using DurationType  = Clock::duration;
    using TimePointType = Clock::time_point;

protected:
    DurationType  _elapsed;
    DurationType  _total_elapsed;
    TimePointType _last_update;

public:
    /// @brief Default constructor, set elapsed time to 0.
    Timer(): _elapsed(DurationType(0)) {}

    /// @brief Record the start time.
    void start() {
        _last_update = Clock::now();
    }

    /// @brief Record the stop time and calculate the elapsed time.
    void stop() {
        auto end = Clock::now();
        _elapsed = (end - _last_update);
        _total_elapsed += _elapsed;
    }
    
    /// @brief Get the last elapsed time.
    /// @return A Clock::duration object records the last elapsed time.
    DurationType elapsed() const {
        return _elapsed;
    }

    /// @brief Get the total elapsed time.
    /// @return A Clock::duration object records the total elapsed time.
    DurationType total_elapsed() const {
        return _total_elapsed;
    }
};

/// @class TimeGuard
/// @brief The guard class of Timer class, used to record time automatically.
class TimerGuard {
protected:
    Timer& _timer;

public:
    /// @brief Default constructor, record start time automatically.
    TimerGuard(Timer& timer):_timer(timer) {
        _timer.start();
    }

    /// @brief Default destroyer, record end time automatically.
    ~TimerGuard() {
        _timer.stop();
    }
};
