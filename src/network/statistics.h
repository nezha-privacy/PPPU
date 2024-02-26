#pragma once

#include <cstdio>
#include <iostream>
#include <chrono>

#include "playerid.h"
#include "bitrate.hpp"
#include "../tools/timer.h"
#include "../tools/byte_vector.h"

namespace network {

/// @struct Statistics
/// @brief Record the number of bytes sent and received in this communication.
struct Statistics
{
    using size_type    = std::size_t;
    using DurationType = Timer::DurationType;

    std::vector<size_type>    bytes_send;     // bytes send to   player i
    std::vector<size_type>    bytes_recv;     // bytes recv from player i
    std::vector<DurationType> elapsed_send;   // time comsumed to send to   player i
    std::vector<DurationType> elapsed_recv;   // time consumed to recv from player i
    DurationType              elapsed_total;  // time consumed on networking operations(blocking)

};

} // namespace network
