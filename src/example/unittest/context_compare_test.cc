#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <thread>

#include "context/visibility.h"
#include "context/value.hpp"
#include "context/compare/compare.hpp"
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


float get_less_ans(float data_1, float data_2) { if(data_1 < data_2) return true; else return false; }
float get_greater_ans(float data_1, float data_2) { if(data_1 > data_2) return true; else return false; }
float get_less_equal_ans(float data_1, float data_2) { if(data_1 <= data_2) return true; else return false; }
float get_greater_equal_ans(float data_1, float data_2) { if(data_1 >= data_2) return true; else return false; }
float get_equal_to_ans(float data_1, float data_2) { if(data_1 == data_2) return true; else return false; }
float get_not_equal_to_ans(float data_1, float data_2) { if(data_1 != data_2) return true; else return false; }
float get_min_ans(float data_1, float data_2) { if(data_1 < data_2) return data_1; else return data_2; }
float get_max_ans(float data_1, float data_2) { if(data_1 > data_2) return data_1; else return data_2; }

#define TEST_BINARY_FUNC(op, vis_1, vis_2)                                                      \
TEST(ContextCompareBinaryTest, op_##op##_##vis_1##vis_2) {                                      \
    float epsilon = 0.000001f;                                                                  \
    float data_1 = rand_f();                                                                    \
    float data_2 = rand_f();                                                                    \
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
    EXPECT_NEAR(get_data(result), get_##op##_ans(data_1, data_2), epsilon);                     \
    thread_player1.join();                                                                      \
}


#define TEST_BINARY_FUNC_WITH_TYPE(op) \
    TEST_BINARY_FUNC(op, pu, pu)     \
    TEST_BINARY_FUNC(op, pu, pr)     \
    TEST_BINARY_FUNC(op, pu, sh)     \
    TEST_BINARY_FUNC(op, pr, pu)     \
    TEST_BINARY_FUNC(op, pr, pr)     \
    TEST_BINARY_FUNC(op, pr, sh)     \
    TEST_BINARY_FUNC(op, sh, pu)     \
    TEST_BINARY_FUNC(op, sh, pr)     \
    TEST_BINARY_FUNC(op, sh, sh)    

TEST_BINARY_FUNC_WITH_TYPE(less)
TEST_BINARY_FUNC_WITH_TYPE(greater)
TEST_BINARY_FUNC_WITH_TYPE(less_equal)
TEST_BINARY_FUNC_WITH_TYPE(greater_equal)
TEST_BINARY_FUNC_WITH_TYPE(equal_to)
TEST_BINARY_FUNC_WITH_TYPE(not_equal_to)
TEST_BINARY_FUNC_WITH_TYPE(min)
TEST_BINARY_FUNC_WITH_TYPE(max)

int main() {
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
