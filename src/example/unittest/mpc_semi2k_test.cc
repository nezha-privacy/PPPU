#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <thread>

#include "context/visibility.h"
#include "context/value.hpp"
#include "context/basic/basic.hpp"
#include "context/basic/raw.hpp"
#include "datatypes/Z2k.hpp"
#include "mpc/semi2k/semi2k.hpp"
#include "ndarray/ndarray_ref.hpp"

#include "example/utils.hpp"
#include "example/utils.cpp"

#include <gtest/gtest.h>

using tcp = boost::asio::ip::tcp;
using address = boost::asio::ip::address;
using Z = Z2<128, true>;

template<size_t K, bool Signed>
void print_array(const core::ArrayRef<Z2<K, Signed>>& vec){
    std::cout << "(";
    for(std::size_t i = 0; i != vec.numel(); ++i){
        std::cout << vec[i].to_string() << ", ";
    }
    std::cout << ")" << std::endl;
}

core::ArrayRef<Z> make_array_alpha(int pid) {
    core::ArrayRef<Z> arr = core::make_array(std::vector<Z>(10));
    if(pid == 0){
        for(std::size_t i = 0; i != arr.numel(); ++i) arr[i] = i;
    }
    else{
        for(std::size_t i = 0; i != arr.numel(); ++i) arr[i] = 20 - i;
    }
    return arr;
}

core::ArrayRef<Z> make_array_beta(int pid) {
    core::ArrayRef<Z> arr = core::make_array(std::vector<Z>(10));
    if(pid == 0){
        for(std::size_t i = 0; i != arr.numel(); ++i) arr[i] = 20 - i;
    }
    else{
        for(std::size_t i = 0; i != arr.numel(); ++i) arr[i] = i + 10;
    }
    return arr;
}

core::ArrayRef<Z> make_array_gamma(int pid) {
    if(pid == 0){
        core::ArrayRef<Z> arr = core::make_array(std::vector<Z>(1));
        arr[0] = rand_32();
        return arr;
    }
    else{
        core::ArrayRef<Z> arr = core::make_array(std::vector<Z>(1));
        arr[0] = 0;
        return arr;
    }
}

float get_input_ans(float x) { return x; }
float get_open_ans(float x) { return 30; }
float get_neg_ans(float x) { return -x; }
float get_msb_ans(float x) { if(x < 0) return 1; else return 0; }
float get_eqz_ans(float x) { if(x == 0) return 1; else return 0; }

#define TEST_UNARY_FUNC(op, vis)                                                                           \
TEST(MPCSemi2kUnaryTest, op_##op##_##vis) {                                                                \
    int base_port = 8888;                                                                                  \
    std::vector<tcp::endpoint> endpoints;                                                                  \
    for(int i = 0; i < 2; ++i) {                                                                           \
        endpoints.emplace_back(                                                                            \
            address::from_string("127.0.0.1"),                                                             \
            base_port+i                                                                                    \
        );                                                                                                 \
    }                                                                                                      \
    auto thread_player1 = std::thread([&]() {                                                              \
        int pid = 1;                                                                                       \
        int n_players = 2;                                                                                 \
        network::PlainMultiPartyPlayer player(pid, n_players);                                             \
        player.run(2);                                                                                     \
        player.connect(endpoints);                                                                         \
        mpc::Semi2kTriple semi2k_triple;                                                                   \
        mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);                                       \
        core::ArrayRef<Z> arr = make_array_beta(pid);                                                      \
        core::ArrayRef<Z> ans = semi2k.op##_##vis(arr);                                                    \
    });                                                                                                    \
    int pid = 0;                                                                                           \
    int n_players = 2;                                                                                     \
    network::PlainMultiPartyPlayer player(pid, n_players);                                                 \
    player.run(2);                                                                                         \
    player.connect(endpoints);                                                                             \
    mpc::Semi2kTriple semi2k_triple;                                                                       \
    mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);                                           \
    core::ArrayRef<Z> arr = make_array_beta(pid);                                                          \
    core::ArrayRef<Z> ans = semi2k.op##_##vis(arr);                                                        \
    if(ans.numel() == 1) {                                                                                 \
        EXPECT_FLOAT_EQ(std::stof(ans[0].to_string()), get_##op##_ans(std::stof(arr[0].to_string())));     \
    }                                                                                                      \
    else {                                                                                                 \
        for(int i = 0; i < 10; i++){                                                                       \
            EXPECT_FLOAT_EQ(std::stof(ans[i].to_string()), get_##op##_ans(std::stof(arr[i].to_string()))); \
        }                                                                                                  \
    }                                                                                                      \
    thread_player1.join();                                                                                 \
}

TEST_UNARY_FUNC(input, p)
TEST_UNARY_FUNC(open, s)
TEST_UNARY_FUNC(neg, p)
TEST_UNARY_FUNC(neg, s)
TEST_UNARY_FUNC(msb, p)
TEST_UNARY_FUNC(msb, s)
TEST_UNARY_FUNC(eqz, p)
TEST_UNARY_FUNC(eqz, s)

float get_add_pp_ans(int pid, float x, float y) { return x + y; }
float get_add_sp_ans(int pid, float x, float y) { if(pid == 0) return x + y; else return x; }
float get_add_ss_ans(int pid, float x, float y) { if(pid == 0) return 20; else return 30; }
float get_mul_pp_ans(int pid, float x, float y) { return x * y; }
float get_mul_sp_ans(int pid, float x, float y) { return x * y; }
float get_mul_ss_ans(int pid, float x, float y) { if(pid == 0) return 600; else return 0; }

#define TEST_BINARY_FUNC(op, vis1, vis2)                                                                                                                        \
TEST(MPCSemi2kBinaryTest, op_##op##_##vis1##vis2) {                                                                                                             \
    int base_port = 8888;                                                                                                                                       \
    std::vector<tcp::endpoint> endpoints;                                                                                                                       \
    for(int i = 0; i < 2; ++i) {                                                                                                                                \
        endpoints.emplace_back(                                                                                                                                 \
            address::from_string("127.0.0.1"),                                                                                                                  \
            base_port+i                                                                                                                                         \
        );                                                                                                                                                      \
    }                                                                                                                                                           \
    auto thread_player1 = std::thread([&]() {                                                                                                                   \
        int pid = 1;                                                                                                                                            \
        int n_players = 2;                                                                                                                                      \
        network::PlainMultiPartyPlayer player(pid, n_players);                                                                                                  \
        player.run(2);                                                                                                                                          \
        player.connect(endpoints);                                                                                                                              \
        mpc::Semi2kTriple semi2k_triple;                                                                                                                        \
        mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);                                                                                            \
        core::ArrayRef<Z> arr1 = make_array_alpha(pid);                                                                                                         \
        core::ArrayRef<Z> arr2 = make_array_beta(pid);                                                                                                          \
        core::ArrayRef<Z> ans = semi2k.op##_##vis1##vis2(arr1, arr2);                                                                                           \
        for(int i = 0; i < 10; i++){                                                                                                                            \
            EXPECT_FLOAT_EQ(std::stof(ans[i].to_string()), get_##op##_##vis1##vis2##_ans(pid, std::stof(arr1[i].to_string()), std::stof(arr2[i].to_string()))); \
        }                                                                                                                                                       \
    });                                                                                                                                                         \
    int pid = 0;                                                                                                                                                \
    int n_players = 2;                                                                                                                                          \
    network::PlainMultiPartyPlayer player(pid, n_players);                                                                                                      \
    player.run(2);                                                                                                                                              \
    player.connect(endpoints);                                                                                                                                  \
    mpc::Semi2kTriple semi2k_triple;                                                                                                                            \
    mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);                                                                                                \
    core::ArrayRef<Z> arr1 = make_array_alpha(pid);                                                                                                             \
    core::ArrayRef<Z> arr2 = make_array_beta(pid);                                                                                                              \
    core::ArrayRef<Z> ans = semi2k.op##_##vis1##vis2(arr1, arr2);                                                                                               \
    for(int i = 0; i < 10; i++){                                                                                                                                \
        EXPECT_FLOAT_EQ(std::stof(ans[i].to_string()), get_##op##_##vis1##vis2##_ans(pid, std::stof(arr1[i].to_string()), std::stof(arr2[i].to_string())));     \
    }                                                                                                                                                           \
    thread_player1.join();                                                                                                                                      \
}

TEST_BINARY_FUNC(add, p, p)
TEST_BINARY_FUNC(add, s, p)
TEST_BINARY_FUNC(add, s, s)
TEST_BINARY_FUNC(mul, p, p)
TEST_BINARY_FUNC(mul, s, p)
TEST_BINARY_FUNC(mul, s, s)

float get_lshift_ans(float x) { return (int)x << 2; }
float get_trunc_ans(float x) { return (int)x >> 2; }

#define TEST_SHIFT_FUNC(op, vis)                                                                       \
TEST(MPCSemi2kShiftTest, op_##op##_##vis) {                                                            \
    int base_port = 8888;                                                                              \
    std::vector<tcp::endpoint> endpoints;                                                              \
    for(int i = 0; i < 2; ++i) {                                                                       \
        endpoints.emplace_back(                                                                        \
            address::from_string("127.0.0.1"),                                                         \
            base_port+i                                                                                \
        );                                                                                             \
    }                                                                                                  \
    auto thread_player1 = std::thread([&]() {                                                          \
        int pid = 1;                                                                                   \
        int n_players = 2;                                                                             \
        network::PlainMultiPartyPlayer player(pid, n_players);                                         \
        player.run(2);                                                                                 \
        player.connect(endpoints);                                                                     \
        mpc::Semi2kTriple semi2k_triple;                                                               \
        mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);                                   \
        core::ArrayRef<Z> arr = make_array_alpha(pid);                                                 \
        core::ArrayRef<Z> ans = semi2k.op##_##vis(arr, 2);                                             \
    });                                                                                                \
    int pid = 0;                                                                                       \
    int n_players = 2;                                                                                 \
    network::PlainMultiPartyPlayer player(pid, n_players);                                             \
    player.run(2);                                                                                     \
    player.connect(endpoints);                                                                         \
    mpc::Semi2kTriple semi2k_triple;                                                                   \
    mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);                                       \
    core::ArrayRef<Z> arr = make_array_alpha(pid);                                                     \
    core::ArrayRef<Z> ans = semi2k.op##_##vis(arr, 2);                                                 \
    for(int i = 0; i < 10; i++){                                                                       \
        EXPECT_FLOAT_EQ(std::stof(ans[i].to_string()), get_##op##_ans(std::stof(arr[i].to_string()))); \
    }                                                                                                  \
    thread_player1.join();                                                                             \
}

TEST_SHIFT_FUNC(lshift, p)
TEST_SHIFT_FUNC(lshift, s)
TEST_SHIFT_FUNC(trunc, p)
TEST_SHIFT_FUNC(trunc, s)

std::vector<int> bitdec(int n) {
    bool is_positive = true;
	std::string str="";
    std::vector<int> vec;
    if(n < 0) { is_positive = false; n = -n; }
	while (n!=0) { str = std::to_string(n % 2) + str; n = n / 2; }
    if(!is_positive) vec.push_back(1); else vec.push_back(0);
    for(int i = str.size(); i < 127; i++) vec.push_back(0);
    for(int i = 0; i < str.size(); i++) vec.push_back(str[i] - '0');
    if(!is_positive) {
        for(int i = 1; i < 128; i++) { if(vec[i] == 0) vec[i] = 1; else vec[i] = 0; }
        if(vec[127] == 0) vec[127] = 1;
        else {
            vec[127] = 0;
            int i = 126;
            while(vec[i] == 1) { vec[i] = 0; i--; }
            vec[i] = 1;
        }
    }
    vec.push_back(str.size());
	return vec;
}

std::vector<int> get_bitdec_ans(int data) { auto ans = bitdec(data); ans.pop_back(); return ans; }
std::vector<int> get_h1bitdec_ans(int data) {
    auto ans = bitdec(data); 
    for(int i = 0; i < ans.size(); i++) 
        if(i != 128 - ans[128]) ans[i] = 0;
    if(data < 0) ans[0] = 1; 
    ans.pop_back();
    return ans; 
}

#define TEST_BITDEC_FUNC(op, vis)                                         \
TEST(MPCSemi2kBitdecTest, op_##op##_##vis) {                              \
    int base_port = 8888;                                                 \
    std::vector<tcp::endpoint> endpoints;                                 \
    for(int i = 0; i < 2; ++i) {                                          \
        endpoints.emplace_back(                                           \
            address::from_string("127.0.0.1"),                            \
            base_port+i                                                   \
        );                                                                \
    }                                                                     \
    auto thread_player1 = std::thread([&]() {                             \
        int pid = 1;                                                      \
        int n_players = 2;                                                \
        network::PlainMultiPartyPlayer player(pid, n_players);            \
        player.run(2);                                                    \
        player.connect(endpoints);                                        \
        mpc::Semi2kTriple semi2k_triple;                                  \
        mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);      \
        core::ArrayRef<Z> arr = make_array_gamma(pid);                    \
        std::vector<core::ArrayRef<Z>> ans = semi2k.op##_##vis(arr, 128); \
    });                                                                   \
    int pid = 0;                                                          \
    int n_players = 2;                                                    \
    network::PlainMultiPartyPlayer player(pid, n_players);                \
    player.run(2);                                                        \
    player.connect(endpoints);                                            \
    mpc::Semi2kTriple semi2k_triple;                                      \
    mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);          \
    core::ArrayRef<Z> arr = make_array_gamma(pid);                        \
    std::vector<core::ArrayRef<Z>> ans = semi2k.op##_##vis(arr, 128);     \
    std::vector<int> res = get_##op##_ans(std::stoi(arr[0].to_string())); \
    for(int i = 0; i < 128; i++){                                         \
        EXPECT_FLOAT_EQ(std::stof(ans[127 - i][0].to_string()), res[i]);  \
    }                                                                     \
    thread_player1.join();                                                \
}

TEST_BITDEC_FUNC(bitdec, p)
TEST_BITDEC_FUNC(bitdec, s)
TEST_BITDEC_FUNC(h1bitdec, p)
TEST_BITDEC_FUNC(h1bitdec, s)

float get_matmul_pp_ans(int i) { if(i==0) return 60; if(i==1) return 70; if(i==2) return 160; if(i==3) return 195; }
float get_matmul_sp_ans(int i) { return get_matmul_pp_ans(i); }
float get_matmul_ss_ans(int i) { return 2000; }

#define TEST_MATRIX_FUNC(op, vis1, vis2)                                                               \
TEST(MPCSemi2kMatrixTest, op_##op##_##vis1##vis2) {                                                    \
    int base_port = 8888;                                                                              \
    std::vector<tcp::endpoint> endpoints;                                                              \
    for(int i = 0; i < 2; ++i) {                                                                       \
        endpoints.emplace_back(                                                                        \
            address::from_string("127.0.0.1"),                                                         \
            base_port+i                                                                                \
        );                                                                                             \
    }                                                                                                  \
    auto thread_player1 = std::thread([&]() {                                                          \
        int pid = 1;                                                                                   \
        int n_players = 2;                                                                             \
        network::PlainMultiPartyPlayer player(pid, n_players);                                         \
        player.run(2);                                                                                 \
        player.connect(endpoints);                                                                     \
        mpc::Semi2kTriple semi2k_triple;                                                               \
        mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);                                   \
        core::ArrayRef<Z> arr = make_array_alpha(pid);                                                 \
        core::ArrayRef<Z> ans = semi2k.op##_##vis1##vis2(arr, arr, 2, 5, 2);                           \
    });                                                                                                \
    int pid = 0;                                                                                       \
    int n_players = 2;                                                                                 \
    network::PlainMultiPartyPlayer player(pid, n_players);                                             \
    player.run(2);                                                                                     \
    player.connect(endpoints);                                                                         \
    mpc::Semi2kTriple semi2k_triple;                                                                   \
    mpc::Semi2k semi2k(pid, n_players, &player, &semi2k_triple);                                       \
    core::ArrayRef<Z> arr = make_array_alpha(pid);                                                     \
    core::ArrayRef<Z> ans = semi2k.op##_##vis1##vis2(arr, arr, 2, 5, 2);                               \
    for(int i = 0; i < 4; i++){                                                                        \
        EXPECT_FLOAT_EQ(std::stof(ans[i].to_string()), get_##op##_##vis1##vis2##_ans(i));              \
    }                                                                                                  \
    thread_player1.join();                                                                             \
}

TEST_MATRIX_FUNC(matmul, p, p)
TEST_MATRIX_FUNC(matmul, s, p)
TEST_MATRIX_FUNC(matmul, s, s)