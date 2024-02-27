#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <cstdlib>
#include <math.h>
#include <thread>

#include "tools/byte_vector.h"

#include "tutorial_1_prepare.cpp"

namespace tutorial {

// Make ByteVector
ByteVector make_bytevector() { 
    int i = 123;
    ByteVector res;
    res.push_back(&i, sizeof(i));
    return res;
}

// Use to compare two ByteVectors
bool compare_bytevector(ByteVector &a, ByteVector &b) {
    int i = 0;
    std::byte zero = (std::byte)i;
    while(true) {
        if(a.at(i) != b.at(i)) return false;
        else if(a.at(i) == zero && b.at(i) == zero) break;
        else i++;
    }
    return true;
}

void tutorial_4_network(int pid, int num_parties) {
    // Prepare network
    auto netio = make_netio(pid, num_parties, "");
    auto player = std::move(netio).get();

    // Prepare message
    ByteVector msg_send = make_bytevector();
    ByteVector msg_compare = make_bytevector();

    // Send or receive message
    if(pid == 0) {
        ByteVector msg_recv = player->recv(1);
        fmt::print("{}", compare_bytevector(msg_compare, msg_recv));
    }
    else if(pid == 1) {
        player->send(0, std::move(msg_send));
    }
}

}
