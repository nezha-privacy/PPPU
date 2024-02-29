#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <thread>
#include <cmath>

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

template<typename dtype>
std::vector<dtype> arange(dtype start, dtype stop, dtype step = 1) {
    std::vector<dtype> result;
    for (dtype i = start; i < stop; i += step) {
        result.push_back(i);
    }
    return result;
}

float get_data(Value input) {
    if(input.is_plain()) {
        auto data_ndarray = pppu::detail::decode([](auto x){return double(x);}, input.data_p(), input.fracbits());
        // return std::stof(data_ndarray.to_string());
        return data_ndarray.elem({0});
    }
    else {
        auto data_ndarray = pppu::detail::decode([](auto x){return double(x);}, input.data_s(), input.fracbits());
        // return std::stof(data_ndarray.to_string());
        return data_ndarray.elem({0});
    }
}


core::NDArrayRef<double> get_data_ND(Value input) {
    if(input.is_plain()) {
        auto data_ndarray = pppu::detail::decode([](auto x){return double(x);}, input.data_p(), input.fracbits());
        // return std::stof(data_ndarray.to_string());
        return data_ndarray;
    }
    else {
        auto data_ndarray = pppu::detail::decode([](auto x){return double(x);}, input.data_s(), input.fracbits());
        // return std::stof(data_ndarray.to_string());
        return data_ndarray;
    }
}

double get_exp_ans(double data) { return std::exp(data); }
double get_exp2_ans(double data) { return std::exp2(data); }
double get_log_ans(double data) { return std::log(data); }
double get_log2_ans(double data) { return std::log2(data); }
double get_log10_ans(double data) { return std::log10(data); }
double get_floor_ans(double data) { return std::floor(data); }
double get_ceil_ans(double data) { return std::ceil(data); }
double get_round_ans(double data) { return std::round(data); }
double get_sqrt_ans(double data) { return std::sqrt(data); }
double get_sigmoid_ans(double data) { return 1 / (1 + exp(-data)); }

float rand_exp(){return rand_f();}
float rand_exp2(){return rand_exp();}
float rand_log(){return abs(rand_f());}
float rand_log2(){return rand_log();}
float rand_log10(){return rand_log();}
float rand_ceil(){return rand_f();}
float rand_round(){return rand_ceil();}
float rand_floor(){return rand_ceil();}
float rand_sqrt(){return abs(rand_f());}
float rand_sigmoid(){return rand_f();}

auto exp_arange(){return arange<double>(-22, 22, 0.5);}
auto exp2_arange(){return arange<double>(-31, 31, 0.5);}
auto log_arange(){return arange<double>(0.1, 10, 0.2);}
auto log2_arange(){return arange<double>(0.1, 10, 0.2);}
auto log10_arange(){return arange<double>(0.1, 10, 0.2);}
auto ceil_arange(){return arange<double>(-10.0, 10, 0.2);}
auto round_arange(){return arange<double>(-10.0, 10, 0.2);}
auto floor_arange(){return arange<double>(-10.0, 10, 0.2);}
auto sqrt_arange(){return arange<double>(0.5, 30, 0.5);}
auto sigmoid_arange(){return arange<double>(-22, 22, 0.5);}

#define TEST_UNARY_FUNC(op)                                                               \
TEST(ContextMathUnaryTest, op_##op) {                                                     \
    float epsilon = 0.0001f;                                                              \
    auto data = op##_arange();                                                            \                                           
    auto thread_player1 = std::thread([&]() {                                             \
        auto context = run_player(1, 2);                                                  \
        pppu::Context* ctx = context.get();                                               \
        Value input = make_value_vec<std::vector<double>, Value>(ctx, 1, data, pppu::Visibility::Public(), -1);     \
        Value result = pppu::op(ctx, input);                                              \
    });                                                                                   \
    auto context = run_player(0, 2);                                                      \
    pppu::Context* ctx = context.get();                                                   \
    Value input = make_value_vec<std::vector<double>, Value>(ctx, 0, data, pppu::Visibility::Public(), -1);         \
    Value result = pppu::op(ctx, input);                                                  \
    for(int i = 0; i < data.size(); ++i){                                                 \
    std::cout << data[i] << " " << double(get_data_ND(result).elem({i})) << std::endl;    \
    double relative_error = (double(get_data_ND(result).elem({i}))-get_##op##_ans(data[i]))/get_##op##_ans(data[i]);\
    EXPECT_NEAR(double(get_data_ND(result).elem({i})), get_##op##_ans(data[i]), 0.001);   \
    }                                                                                     \
    thread_player1.join();                                                                \
}
 

// TEST_UNARY_FUNC(exp) 
// TEST_UNARY_FUNC(exp2) 
// TEST_UNARY_FUNC(log) 
// TEST_UNARY_FUNC(log2) 
// TEST_UNARY_FUNC(log10) 
// TEST_UNARY_FUNC(floor) 
// TEST_UNARY_FUNC(ceil) 
// TEST_UNARY_FUNC(round) 
// TEST_UNARY_FUNC(sqrt)  
// TEST_UNARY_FUNC(sigmoid)    



float get_div_ans(float data_1, float data_2) { return data_1 / data_2; }
float get_pow_ans(float data_1, int data_2){ return pow(data_1, data_2); }


auto div_data_1(){return arange<double>(-10, 9.9, 0.5);}
auto div_data_2(){return arange<double>(-10, 13.9, 0.6);}


#define TEST_BINARY_FUNC(op)                                                                    \
TEST(ContextMathBinaryTest, op_##op) {                                                          \
    float epsilon = 0.000001f;                                                                  \
    auto data_1 = op##_data_1();                                                                \
    auto data_2 = op##_data_2();                                                                \
    auto thread_player1 = std::thread([&]() {                                                   \
        auto context = run_player(1, 2);                                                        \
        pppu::Context* ctx = context.get();                                                     \
        Value input_1 = make_value_vec<std::vector<double>, Value>(ctx, 1, data_1, pppu::Visibility::Public(), -1); \
        Value input_2 = make_value_vec<std::vector<double>, Value>(ctx, 1, data_2, pppu::Visibility::Public(), -1); \
        Value result = pppu::op(ctx, input_1, input_2);                                         \
    });                                                                                         \
    auto context = run_player(0, 2);                                                            \
    pppu::Context* ctx = context.get();                                                         \
    Value input_1 = make_value_vec<std::vector<double>, Value>(ctx, 0, data_1, pppu::Visibility::Public(), -1);     \
    Value input_2 = make_value_vec<std::vector<double>, Value>(ctx, 0, data_2, pppu::Visibility::Public(), -1);     \
    Value result = pppu::op(ctx, input_1, input_2);                                             \
    for(int i = 0; i < data_1.size(); ++i){                                                     \
        std::cout << data_1[i] << " " << data_2[i] << std::endl;                                \
        EXPECT_NEAR(get_data_ND(result).elem({i}), get_##op##_ans(data_1[i], data_2[i]), epsilon);                  \
    }                                                                                           \
    thread_player1.join();                                                                      \
}

auto pow_data_1(){return arange<double>(-5, 5, 0.5);}
auto pow_data_2(){return arange<int>(-3, 3);}

#define TEST_BINARY_POW(op)                                                                     \
TEST(ContextMathBinaryTest, op_##op) {                                                          \
    float epsilon = 0.00001f;                                                                   \
    auto data_1 = op##_data_1();                                                                \
    auto data_2 = op##_data_2();                                                                \
    auto thread_player1 = std::thread([&]() {                                                   \
        auto context = run_player(1, 2);                                                        \
        pppu::Context* ctx = context.get();                                                     \
        Value input_1 = make_value_vec<std::vector<double>, Value>(ctx, 1, data_1, pppu::Visibility::Public(), -1); \
        for(int i = 0; i < data_2.size(); ++i){                                                 \
        Value result = pppu::op(ctx, input_1, data_2[i]);                                       \
    }                                                                                           \
    });                                                                                         \
    auto context = run_player(0, 2);                                                            \
    pppu::Context* ctx = context.get();                                                         \
    Value input_1 = make_value_vec<std::vector<double>, Value>(ctx, 0, data_1, pppu::Visibility::Public(), -1);     \
    for(int i = 0; i < data_2.size(); ++i){                                                     \
        Value result = pppu::op(ctx, input_1, data_2[i]);                                       \
        for(int j = 0; j < data_1.size(); ++j){                                                 \
            std::cout << data_1[j] << " " << data_2[i] << std::endl;                            \
            EXPECT_NEAR(get_data_ND(result).elem({j}), get_##op##_ans(data_1[j], data_2[i]), epsilon);               \
        }                                                                                       \
    }                                                                                           \
    thread_player1.join();                                                                      \
}

auto rand_polynomial_1(){
    const int num = 4;
    std::vector<float> coef(num);
    for (int i = 0; i < num; ++i){
        coef.emplace_back(rand_f());
    }
    return coef;
}
float rand_polynomial_2(){return rand_f()/10;}
float get_polynomial_ans(std::vector<float> coef, float x){
    float res = 0;
    for(int i = 0; i < coef.size(); ++i){
        res += coef[i] * pow(x, i);
    }
    return res;
}

#define TEST_BINARY_POLYNOMIAL(op)                                                                \
TEST(ContextMathBinaryTest, op_##op) {                                                            \
    float epsilon = 0.0001f;                                                                      \
    auto data_1 = rand_##op##_1();                                                                \
    auto data_2 = rand_##op##_2();                                                                \
    auto thread_player1 = std::thread([&]() {                                                     \
        auto context = run_player(1, 2);                                                          \
        pppu::Context* ctx = context.get();                                                       \
        std::vector<Value> input_1;                                                               \
        for(int i = 0; i < data_1.size(); ++i){                                                   \
            Value tem = make_value_data<float, Value>(ctx, 1, data_1[i], pppu::Visibility::Public(), -1);\
            input_1.emplace_back(tem);                                                            \
        }                                                                                         \
        Value input_2 = make_value_data<float, Value>(ctx, 1, data_2, pppu::Visibility::Public(), -1);   \
        Value result = pppu::op(ctx, std::span<const Value>(input_1), input_2);                   \
    });                                                                                           \
    auto context = run_player(0, 2);                                                              \
    pppu::Context* ctx = context.get();                                                           \
    std::vector<Value> input_1;                                                                   \
        for(int i = 0; i < data_1.size(); ++i){                                                   \
            Value tem = make_value_data<float, Value>(ctx, 0, data_1[i], pppu::Visibility::Public(), -1);\
            input_1.emplace_back(tem);                                                            \
        }                                                                                         \
    Value input_2 = make_value_data<float, Value>(ctx, 0, data_2, pppu::Visibility::Public(), -1);       \
    Value result = pppu::op(ctx, std::span<const Value>(input_1), input_2);                       \
    EXPECT_NEAR(get_data(result), get_##op##_ans(data_1, data_2), epsilon);                       \
    thread_player1.join();                                                                        \
}

    // TEST_BINARY_FUNC(div)     
    // TEST_BINARY_POW(pow)          
    TEST_BINARY_POLYNOMIAL(polynomial)          


int main() {
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
