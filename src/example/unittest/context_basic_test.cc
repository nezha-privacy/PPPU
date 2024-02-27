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

using ProtocolType = mpc::Semi2k;
using ProtocolTriple = mpc::Semi2kTriple;
using Value = pppu::Value<ProtocolType, Z2<128, true>, Z2<128, true>>;

pppu::Visibility get_pu_vis() { return pppu::Visibility::Public(); }
pppu::Visibility get_pr_vis() { return pppu::Visibility::Private(0); }
pppu::Visibility get_sh_vis() { return pppu::Visibility::Share(); }

float get_data(Value input) {
    if(input.is_plain()) {
        auto data_ndarray = pppu::detail::decode([](auto x){return double(x);}, input.data_p(), input.fracbits());
        return std::stof(data_ndarray.to_string());
    }
    else {
        auto data_ndarray = pppu::detail::decode([](auto x){return double(x);}, input.data_s(), input.fracbits());
        return std::stof(data_ndarray.to_string());
    }
}

float get_input_ans(float data) { return data; }
float get_open_ans(float data) { return data; }
float get_neg_ans(float data) { return - data; }
float get_square_ans(float data) { return data * data; }
float get_msb_ans(float data) { if(data < 0) return 1; else return 0; }
float get_eqz_ans(float data) { if(data == 0) return 1; else return 0; }
float get_sign_ans(float data) { if(data < 0) return -1; else return 1; }
float get_abs_ans(float data) { if(data < 0) return - data; else return data; }

#define TEST_UNARY_FUNC(op, vis)                                                          \
TEST(ContextBasicUnaryTest, op_##op##_##vis) {                                            \
    float data = rand_32();                                                               \
    std::cout<<data<<std::endl;                                                           \
    auto thread_player1 = std::thread([&]() {                                             \
        auto context = run_player(1, 2);                                                  \
        pppu::Context* ctx = context.get();                                               \
        Value input = make_value_data<float, Value>(ctx, 1, data, get_##vis##_vis(), -1); \
        Value result = pppu::op(ctx, input);                                              \
    });                                                                                   \
    auto context = run_player(0, 2);                                                      \
    pppu::Context* ctx = context.get();                                                   \
    Value input = make_value_data<float, Value>(ctx, 0, data, get_##vis##_vis(), -1);     \
    Value result = pppu::op(ctx, input);                                                  \
    EXPECT_FLOAT_EQ(get_data(result), get_##op##_ans(data));                              \
    thread_player1.join();                                                                \
}

TEST_UNARY_FUNC(input, pr)
TEST_UNARY_FUNC(open, sh)
TEST_UNARY_FUNC(neg, pu)
TEST_UNARY_FUNC(neg, pr)
TEST_UNARY_FUNC(neg, sh)
TEST_UNARY_FUNC(square, pu)
TEST_UNARY_FUNC(square, pr)
TEST_UNARY_FUNC(square, sh)
TEST_UNARY_FUNC(msb, pu)
TEST_UNARY_FUNC(msb, pr)
TEST_UNARY_FUNC(msb, sh)
TEST_UNARY_FUNC(eqz, pu)
TEST_UNARY_FUNC(eqz, pr)
TEST_UNARY_FUNC(eqz, sh)
TEST_UNARY_FUNC(sign, pu)
TEST_UNARY_FUNC(sign, pr)
TEST_UNARY_FUNC(sign, sh)
TEST_UNARY_FUNC(abs, pu)
TEST_UNARY_FUNC(abs, pr)
TEST_UNARY_FUNC(abs, sh)

float get_add_ans(float data_1, float data_2) { return data_1 + data_2; }
float get_sub_ans(float data_1, float data_2) { return data_1 - data_2; }
float get_mul_ans(float data_1, float data_2) { return data_1 * data_2; }

#define TEST_BINARY_FUNC(op, vis_1, vis_2)                                                      \
TEST(ContextBasicBinaryTest, op_##op##_##vis_1##vis_2) {                                        \
    float data_1 = rand_32();                                                                   \
    float data_2 = rand_32();                                                                   \
    auto thread_player1 = std::thread([&]() {                                                   \
        auto context = run_player(1, 2);                                                        \
        pppu::Context* ctx = context.get();                                                     \
        Value input_1 = make_value_data<float, Value>(ctx, 1, data_1, get_##vis_1##_vis(), -1); \
        Value input_2 = make_value_data<float, Value>(ctx, 1, data_2, get_##vis_2##_vis(), -1); \
        Value result = pppu::op(ctx, input_1, input_2);                                         \
    });                                                                                         \
    auto context = run_player(0, 2);                                                            \
    pppu::Context* ctx = context.get();                                                         \
    Value input_1 = make_value_data<float, Value>(ctx, 0, data_1, get_##vis_1##_vis(), -1);     \
    Value input_2 = make_value_data<float, Value>(ctx, 0, data_2, get_##vis_2##_vis(), -1);     \
    Value result = pppu::op(ctx, input_1, input_2);                                             \
    EXPECT_FLOAT_EQ(get_data(result), get_##op##_ans(data_1, data_2));                          \
    thread_player1.join();                                                                      \
}

TEST_BINARY_FUNC(add, pu, pu)
TEST_BINARY_FUNC(add, pu, pr)
TEST_BINARY_FUNC(add, pu, sh)
TEST_BINARY_FUNC(add, pr, pu)
TEST_BINARY_FUNC(add, pr, pr)
TEST_BINARY_FUNC(add, pr, sh)
TEST_BINARY_FUNC(add, sh, pu)
TEST_BINARY_FUNC(add, sh, pr)
TEST_BINARY_FUNC(add, sh, sh)
TEST_BINARY_FUNC(sub, pu, pu)
TEST_BINARY_FUNC(sub, pu, pr)
TEST_BINARY_FUNC(sub, pu, sh)
TEST_BINARY_FUNC(sub, pr, pu)
TEST_BINARY_FUNC(sub, pr, pr)
TEST_BINARY_FUNC(sub, pr, sh)
TEST_BINARY_FUNC(sub, sh, pu)
TEST_BINARY_FUNC(sub, sh, pr)
TEST_BINARY_FUNC(sub, sh, sh)
TEST_BINARY_FUNC(mul, pu, pu)
TEST_BINARY_FUNC(mul, pu, pr)
TEST_BINARY_FUNC(mul, pu, sh)
TEST_BINARY_FUNC(mul, pr, pu)
TEST_BINARY_FUNC(mul, pr, pr)
TEST_BINARY_FUNC(mul, pr, sh)
TEST_BINARY_FUNC(mul, sh, pu)
TEST_BINARY_FUNC(mul, sh, pr)
TEST_BINARY_FUNC(mul, sh, sh)

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

#define TEST_BITDEC_FUNC(op, vis)                                                   \
TEST(ContextBasicBitdecTest, op_##op##_##vis) {                                     \
    int data = rand_32();                                                           \
    std::cout<<data<<std::endl;                                                     \
    auto thread_player1 = std::thread([&]() {                                       \
        auto context = run_player(1, 2);                                            \
        pppu::Context* ctx = context.get();                                         \
        std::vector<int> ans = get_##op##_ans(data);                                \
        Value input = make_value_data<int, Value>(ctx, 1, data, get_##vis##_vis()); \
        std::vector<Value> result = pppu::op(ctx, input, 128);                      \
    });                                                                             \
    auto context = run_player(0, 2);                                                \
    pppu::Context* ctx = context.get();                                             \
    std::vector<int> ans = get_##op##_ans(data);                                    \
    for(int i = 0; i < ans.size(); i++) std::cout<<ans[i];                          \
    std::cout<<std::endl;                                                           \
    Value input = make_value_data<int, Value>(ctx, 0, data, get_##vis##_vis());     \
    std::vector<Value> result = pppu::op(ctx, input, 128);                          \
    for(int i = 0; i < ans.size(); i++) {                                           \
        std::cout<<get_data(result[127 - i]);                                       \
        EXPECT_EQ(get_data(result[127 - i]), ans[i]);                               \
    }std::cout<<"\n";                                                               \
    thread_player1.join();                                                          \
}

TEST_BITDEC_FUNC(bitdec, pu)
TEST_BITDEC_FUNC(bitdec, pr)
TEST_BITDEC_FUNC(bitdec, sh)
TEST_BITDEC_FUNC(h1bitdec, pu)
TEST_BITDEC_FUNC(h1bitdec, pr)
TEST_BITDEC_FUNC(h1bitdec, sh)

#define TEST_BITCOMP_FUNC(op, vis)                                                  \
TEST(ContextBasicBitcompTest, op_##op##_##vis) {                                    \
    int data = rand_32();                                                           \
    std::cout<<data<<std::endl;                                                     \
    auto thread_player1 = std::thread([&]() {                                       \
        auto context = run_player(1, 2);                                            \
        pppu::Context* ctx = context.get();                                         \
        std::vector<int> ans = get_bitdec_ans(data);                                \
        Value input = make_value_data<int, Value>(ctx, 1, data, get_##vis##_vis()); \
        std::vector<Value> tmp_v = pppu::bitdec(ctx, input, 128);                   \
        std::span<const Value> tmp(tmp_v);                                          \
        Value result = pppu::bitcomp(ctx, tmp, 128);                                \
    });                                                                             \
    auto context = run_player(0, 2);                                                \
    pppu::Context* ctx = context.get();                                             \
    std::vector<int> ans = get_bitdec_ans(data);                                    \
    for(int i = 0; i < ans.size(); i++) std::cout<<ans[i];                          \
    std::cout<<std::endl;                                                           \
    Value input = make_value_data<int, Value>(ctx, 0, data, get_##vis##_vis());     \
    std::vector<Value> tmp_v = pppu::bitdec(ctx, input, 128);                       \
    for(int i=0;i<128;i++) std::cout<<get_data(tmp_v[i]);                           \
    std::span<const Value> tmp(tmp_v);                                              \
    Value result = pppu::bitcomp(ctx, tmp, 0);                                      \
    EXPECT_FLOAT_EQ(get_data(result), data);                                        \
    thread_player1.join();                                                          \
}

TEST_BITCOMP_FUNC(bitcomp, pu)
TEST_BITCOMP_FUNC(bitcomp, pr)
TEST_BITCOMP_FUNC(bitcomp, sh)

Value make_matrix(pppu::Context* ctx, int pid, pppu::Visibility vis) {
    std::vector<int> vec;
    if(pid == 0){
        for(std::size_t i = 0; i != 10; ++i) vec.push_back(i);
    }
    else{
        for(std::size_t i = 0; i != 10; ++i) vec.push_back(20 - i);
    }
    if(pid == 0) {
        core::NDArrayRef<int> arr = core::make_ndarray(vec);
        auto res = pppu::make_private<Value>(ctx, arr);
        return make_visibility<Value>(ctx, res, vis);
    } else {
        auto res = pppu::make_private<Value>(ctx, 0);
        return make_visibility<Value>(ctx, res, vis);
    }
}

std::vector<int> get_matrix(Value input) {
    auto ans_value = input.reshape({1, 4});
    core::NDArrayRef<double> ans_ndarray = core::make_ndarray<double>({1,4});
    if(input.is_plain()) ans_ndarray = pppu::detail::decode([](auto x){return double(x);}, ans_value.data_p(), ans_value.fracbits());
    else ans_ndarray = pppu::detail::decode([](auto x){return double(x);}, ans_value.data_s(), ans_value.fracbits());
    std::string ans_string = ans_ndarray.to_string();
    std::vector<int> ans;
    std::stringstream ss(ans_string);
    std::string item;
	while(std::getline(ss, item, ' ')) ans.push_back(std::stoi(item));
    return ans;
}

float get_matmul_pupu_ans(int i) { if(i==0) return 60; if(i==1) return 70; if(i==2) return 160; if(i==3) return 195; }
float get_matmul_pupr_ans(int i) { return get_matmul_pupu_ans(i); }
float get_matmul_push_ans(int i) { return get_matmul_pupu_ans(i); }
float get_matmul_prpu_ans(int i) { return get_matmul_pupu_ans(i); }
float get_matmul_prpr_ans(int i) { return get_matmul_pupu_ans(i); }
float get_matmul_prsh_ans(int i) { return get_matmul_pupu_ans(i); }
float get_matmul_shpu_ans(int i) { return get_matmul_pupu_ans(i); }
float get_matmul_shpr_ans(int i) { return get_matmul_pupu_ans(i); }
float get_matmul_shsh_ans(int i) { return 2000; }

#define TEST_MATRIX_FUNC(op, vis_1, vis_2)                                            \
TEST(ContextBasicMatrixTest, op_##op##_##vis_1##vis_2) {                              \
    auto thread_player1 = std::thread([&]() {                                         \
        auto context = run_player(1, 2);                                              \
        pppu::Context* ctx = context.get();                                           \
        Value input_1 = make_matrix(ctx, 1, get_##vis_1##_vis());                     \
        Value input_2 = make_matrix(ctx, 1, get_##vis_2##_vis());                     \
        Value result = pppu::op(ctx, input_1.reshape({2,5}), input_2.reshape({5,2})); \
    });                                                                               \
    auto context = run_player(0, 2);                                                  \
    pppu::Context* ctx = context.get();                                               \
    Value input_1 = make_matrix(ctx, 0, get_##vis_1##_vis());                         \
    Value input_2 = make_matrix(ctx, 0, get_##vis_2##_vis());                         \
    Value result = pppu::op(ctx, input_1.reshape({2,5}), input_2.reshape({5,2}));     \
    std::vector<int> ans = get_matrix(result);                                        \
    for(int i=0; i < 4; i++) EXPECT_FLOAT_EQ(get_matmul_pupu_ans(i), ans[i]);         \
    thread_player1.join();                                                            \
}

TEST_MATRIX_FUNC(matmul, pu, pu)
TEST_MATRIX_FUNC(matmul, pu, pr)
TEST_MATRIX_FUNC(matmul, pu, sh)
TEST_MATRIX_FUNC(matmul, pr, pu)
TEST_MATRIX_FUNC(matmul, pr, pr)
TEST_MATRIX_FUNC(matmul, pr, sh)
TEST_MATRIX_FUNC(matmul, sh, pu)
TEST_MATRIX_FUNC(matmul, sh, pr)
TEST_MATRIX_FUNC(matmul, sh, sh)
