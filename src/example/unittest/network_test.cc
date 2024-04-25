#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <random>

#include "network/two_party_player.h"
#include "network/two_party_player.hpp"
#include "network/statistics.h"
#include "tools/byte_vector.h"

#include <gtest/gtest.h>

using size_type = std::size_t;
using Address = boost::asio::ip::address;
using Endpoint = boost::asio::ip::tcp::endpoint;
using network::PlainMultiPartyPlayer;
using namespace std;

ByteVector init_1() { 
    int i = 999;
    ByteVector res;
    res.push_back(&i, sizeof(i));
    return res;
}

ByteVector init_2() { 
    string s = "abc";
    ByteVector res;
    res.push_back(&s, sizeof(s));
    return res;
}

bool compare_byte_vector(ByteVector &a, ByteVector &b) {
    int i = 0;
    byte zero = (byte)i;
    while(true) {
        if(a.at(i) != b.at(i)) {
            return false;
        }
        else if(a.at(i) == zero && b.at(i) == zero) {
            break;
        }
        else {
            i++;
        }
    }
    return true;
}

TEST(NetworkTest, AssertionCommunication) {
    Address local_addr = Address::from_string("127.0.0.1");
    size_type n_threads = 3;
    size_type n_players = 3;
    std::vector<Endpoint> endpoints {
        Endpoint(local_addr, 6666),
        Endpoint(local_addr, 6667),
        Endpoint(local_addr, 6668),
    };

    ByteVector msg_1_compared = init_1();
    ByteVector msg_2_compared = init_2();
    ByteVector msg_1_send = init_1();
    ByteVector msg_1_exchange = init_1();
    ByteVector msg_2_exchange = init_2();
    ByteVector msg_1_broadcast = init_1();
    ByteVector msg_1_passaround = init_1();
    ByteVector msg_2_passaround = init_2();

    auto thread_player1 = std::thread([&]() {
        playerid_t my_pid = 1;
        PlainMultiPartyPlayer player(my_pid, n_players);
        player.run(n_threads);
        player.connect(endpoints);
        //IsRunning
        EXPECT_TRUE(player.is_running());
        //Send
        player.send(0, move(msg_1_send));
        //Statistics
        auto ss = player.get_statistics();
        int send = ss.bytes_send[0];
        EXPECT_EQ(send, 4);
        //Exchange
        ByteVector msg_recv_exchange = player.exchange(0, move(msg_1_exchange));
        EXPECT_TRUE(compare_byte_vector(msg_2_compared, msg_recv_exchange));
        //Broadcast
        player.broadcast(move(msg_1_broadcast));
        //PassAround
        ByteVector msg_recv_passaround = player.pass_around(1,move(msg_2_passaround));
        EXPECT_TRUE(compare_byte_vector(msg_1_compared, msg_recv_passaround));

        auto group = player.all_but_me();
        auto msg = player.mbroadcast_recv(group, move(init_1()));
    });

    auto thread_player2 = std::thread([&]() {
        playerid_t my_pid = 2;
        PlainMultiPartyPlayer player(my_pid, n_players);
        player.run(n_threads);
        player.connect(endpoints);
        //IsRunning
        EXPECT_TRUE(player.is_running());
        //Send
        //Statistics
        //Exchange
        //Broadcast
        ByteVector msg_recv_broadcast = player.recv(1);
        EXPECT_TRUE(compare_byte_vector(msg_1_compared, msg_recv_broadcast));
        //PassAround
        ByteVector msg_recv_passaround = player.recv(1);
        EXPECT_TRUE(compare_byte_vector(msg_2_compared, msg_recv_passaround));

        auto group = player.all_but_me();
        auto msg = player.mbroadcast_recv(group, move(init_1()));
    });
    
    playerid_t my_pid = 0;
    PlainMultiPartyPlayer player(my_pid, n_players);
    player.run(n_threads);
    player.connect(endpoints);
    //IsRunning
    EXPECT_TRUE(player.is_running());
    //Send
    ByteVector msg_recv_send = player.recv(1);
    EXPECT_TRUE(compare_byte_vector(msg_1_compared, msg_recv_send));
    //Statistics
    auto ss = player.get_statistics();
    int recv = ss.bytes_recv[1];
    EXPECT_EQ(recv, 4);
    //Exchange
    ByteVector msg_recv_exchange = player.exchange(1, move(msg_2_exchange));
    EXPECT_TRUE(compare_byte_vector(msg_1_compared, msg_recv_exchange));
    //Broadcast
    ByteVector msg_recv_broadcast = player.recv(1);
    EXPECT_TRUE(compare_byte_vector(msg_1_compared, msg_recv_broadcast));
    //PassAround
    player.send(1,move(msg_1_passaround));

    auto group = player.all_but_me();
    auto msg = player.mbroadcast_recv(group, move(init_1()));

    thread_player1.join();
    thread_player2.join();
}
