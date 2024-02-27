#pragma once

#include <tuple>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <cstdlib>
#include <math.h>
#include <map>

#include "context/visibility.h"
#include "context/value.h"
#include "context/value.hpp"
#include "context/basic/factory.h"
#include "context/basic/factory.hpp"
#include "context/basic/basic.hpp"
#include "context/basic/util.hpp"
#include "context/math/div.hpp"
#include "context/shape/reduce.hpp"
#include "datatypes/Z2k.h"
#include "datatypes/Z2k.hpp"
#include "mpc/protocol.hpp"
#include "mpc/preprocessing.hpp"
#include "mpc/semi2k/semi2k.hpp"
#include "network/network.hpp"
#include "network/multi_party_player.h"
#include "network/multi_party_player.hpp"
#include "ndarray/ndarray_ref.h"
#include "ndarray/ndarray_ref.hpp"
#include "serialization/serialization.hpp"

#include "example/utils.h"
#include "example/utils.hpp"
#include "example/utils.cpp"

#include <benchmark/benchmark.h>

#include <unistd.h>

using ProtocolType = mpc::Semi2k;
using ProtocolTriple = mpc::Semi2kTriple;
using ValueType128 = pppu::Value<ProtocolType, Z2<128, true>, Z2<128, true>>;
using ValueType64 = pppu::Value<ProtocolType, Z2<64, true>, Z2<64, true>>;

pppu::Context* ctx;
long long prev = 0;
std::size_t pid = 0;
std::size_t num_parties = 1;
std::size_t shift = 40;
std::size_t iterations = 10;
bool enable_128 = true;
bool enable_64 = false;
static std::vector<int64_t> sizes={10,20};
static std::vector<int64_t> ops={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};
static std::vector<int64_t> bits={128};
std::map<int, ValueType128> pm128;
std::map<int, ValueType128> sm128;
std::map<int, ValueType128> fpm128;
std::map<int, ValueType128> fsm128;
std::map<int, ValueType64> pm64;
std::map<int, ValueType64> sm64;
std::map<int, ValueType64> fpm64;
std::map<int, ValueType64> fsm64;
std::map<int, std::vector<ValueType128>> pbm128;
std::map<int, std::vector<ValueType128>> sbm128;
std::map<int, std::vector<ValueType64>> pbm64;
std::map<int, std::vector<ValueType64>> sbm64;
ValueType128 p128;
ValueType128 s128;
ValueType128 fp128;
ValueType128 fs128;
ValueType64 p64;
ValueType64 s64;
ValueType64 fp64;
ValueType64 fs64;
std::span<const ValueType128> pb128;
std::span<const ValueType64> pb64;
std::span<const ValueType128> sb128;
std::span<const ValueType64> sb64;
ValueType128 pamat128;
ValueType128 pbmat128;
ValueType64 pamat64;
ValueType64 pbmat64;
ValueType128 samat128;
ValueType128 sbmat128;
ValueType64 samat64;
ValueType64 sbmat64;
ValueType128 out128;
ValueType64 out64;
std::vector<ValueType128> outs128;
std::vector<ValueType64> outs64;

void init(int argc, char** argv) {
    for(int i = 1; i < argc; i++) {
        std::string line = argv[i];
        if(line.substr(0,3) == "pid") pid = std::stoi(line.substr(4));
        else if(line.substr(0,5) == "party") num_parties = std::stoi(line.substr(6));
        else if(line.substr(0,5) == "shift") shift = std::stoi(line.substr(6));
        else if(line.substr(0,4) == "bits") {
            std::stringstream ss(line.substr(5));
            std::string item;
            enable_128 = false;
            bits.clear();
            while (std::getline(ss, item, ',')) {
                if (std::stoi(item) == 64) { enable_64 = true; bits.push_back(std::stoi(item)); }
                else if(std::stoi(item) == 128) { enable_128 = true; bits.push_back(std::stoi(item)); }
            }
        }
        else if(line.substr(0,4) == "size") {
            std::stringstream ss(line.substr(5));
            std::string item;
            sizes.clear();
            while (std::getline(ss, item, ',')) {
                if (!item.empty()) sizes.push_back(std::stoi(item));
            }
        }
        else if(line.substr(0,2) == "op") {
            std::stringstream ss(line.substr(3));
            std::string item;
            ops.clear();
            while (std::getline(ss, item, ',')) {
                if (!item.empty()) ops.push_back(std::stoi(item));
            }
        }
    }
}

void prepare_value(pppu::Context* ctx, std::vector<int64_t> sizes) {
    if(enable_128) {
        for(int i = 0; i < sizes.size(); i++) {
            int size = 1 << sizes[i];
            pm128.insert(std::pair<int, ValueType128>(sizes[i], make_value<int, ValueType128>(ctx, pid, size, pppu::Visibility::Public())));
            sm128.insert(std::pair<int, ValueType128>(sizes[i], make_value<int, ValueType128>(ctx, pid, size, pppu::Visibility::Share())));
            fpm128.insert(std::pair<int, ValueType128>(sizes[i], make_value<double, ValueType128>(ctx, pid, size, pppu::Visibility::Public(), -1)));
            fsm128.insert(std::pair<int, ValueType128>(sizes[i], make_value<double, ValueType128>(ctx, pid, size, pppu::Visibility::Share(), -1)));
            pbm128.insert(std::pair<int, std::vector<ValueType128>>(sizes[i], pppu::bitdec(ctx, pm128[sizes[i]], 128)));
            sbm128.insert(std::pair<int, std::vector<ValueType128>>(sizes[i], pppu::bitdec(ctx, sm128[sizes[i]], 128)));
        }
    }
    if(enable_64) {
        for(int i = 0; i < sizes.size(); i++) {
            int size = 1 << sizes[i];
            pm64.insert(std::pair<int, ValueType64>(sizes[i], make_value<int, ValueType64>(ctx, pid, size, pppu::Visibility::Public())));
            sm64.insert(std::pair<int, ValueType64>(sizes[i], make_value<int, ValueType64>(ctx, pid, size, pppu::Visibility::Share())));
            fpm64.insert(std::pair<int, ValueType64>(sizes[i], make_value<double, ValueType64>(ctx, pid, size, pppu::Visibility::Public(), -1)));
            fsm64.insert(std::pair<int, ValueType64>(sizes[i], make_value<double, ValueType64>(ctx, pid, size, pppu::Visibility::Share(), -1)));
            pbm64.insert(std::pair<int, std::vector<ValueType64>>(sizes[i], pppu::bitdec(ctx, pm64[sizes[i]], 64)));
            sbm64.insert(std::pair<int, std::vector<ValueType64>>(sizes[i], pppu::bitdec(ctx, sm64[sizes[i]], 64)));
        }
    }
}

static void BM_Test(benchmark::State& state) {
    for (auto _ : state) {
        if(state.range(2) == 128) {
            p128 = pm128[state.range(0)];
            s128 = sm128[state.range(0)];
            fp128 = fpm128[state.range(0)];
            fs128 = fsm128[state.range(0)];
            std::span<const ValueType128> pbt128(pbm128[state.range(0)]); pb128 = pbt128;
            std::span<const ValueType128> sbt128(sbm128[state.range(0)]); sb128 = sbt128;
            pamat128 = p128.reshape({1 << state.range(0), 1});
            samat128 = s128.reshape({1 << state.range(0), 1});
            pbmat128 = p128.reshape({1, 1 << state.range(0)});
            sbmat128 = s128.reshape({1, 1 << state.range(0)});
        }
        else if(state.range(2) == 64) {
            p64 = pm64[state.range(0)];
            s64 = sm64[state.range(0)];
            fp64 = fpm64[state.range(0)];
            fs64 = fsm64[state.range(0)];
            std::span<const ValueType64> pbt64(pbm64[state.range(0)]); pb64 = pbt64;
            std::span<const ValueType64> sbt64(sbm64[state.range(0)]); sb64 = sbt64;
            pamat64 = p64.reshape({1 << state.range(0), 1});
            samat64 = s64.reshape({1 << state.range(0), 1});
            pbmat64 = p64.reshape({1, 1 << state.range(0)});
            sbmat64 = s64.reshape({1, 1 << state.range(0)});
        }
        auto start = std::chrono::high_resolution_clock::now();
        switch (state.range(1)) {
            case 0 :
                if(state.range(2) == 128) out128 = pppu::input(ctx, p128);
                else if(state.range(2) == 64) out64 = pppu::input(ctx, p64);
                state.SetLabel("input_p");
                break;
            case 1 :
                if(state.range(2) == 128) out128 = pppu::open(ctx, s128);
                else if(state.range(2) == 64) out64 = pppu::open(ctx, s64);
                state.SetLabel("open_s");
                break;
            case 2 :
                if(state.range(2) == 128) out128 = pppu::neg(ctx, p128);
                else if(state.range(2) == 64) out64 = pppu::neg(ctx, p64);
                state.SetLabel("neg_p");
                break;
            case 3 :
                if(state.range(2) == 128) out128 = pppu::neg(ctx, s128);
                else if(state.range(2) == 64) out64 = pppu::neg(ctx, s64);
                state.SetLabel("neg_s");
                break;
            case 4 : 
                if(state.range(2) == 128) out128 = pppu::add(ctx, p128, p128);
                else if(state.range(2) == 64) out64 = pppu::add(ctx, p64, p64);
                state.SetLabel("add_pp");
                break;
            case 5 : 
                if(state.range(2) == 128) out128 = pppu::add(ctx, p128, s128);
                else if(state.range(2) == 64) out64 = pppu::add(ctx, p64, s64);
                state.SetLabel("add_ps");
                break;
            case 6 : 
                if(state.range(2) == 128) out128 = pppu::add(ctx, s128, s128);
                else if(state.range(2) == 64) out64 = pppu::add(ctx, s64, s64);
                state.SetLabel("add_ss");
                break;
            case 7 : 
                if(state.range(2) == 128) out128 = pppu::sub(ctx, p128, p128);
                else if(state.range(2) == 64) out64 = pppu::sub(ctx, p64, p64);
                state.SetLabel("sub_pp");
                break;
            case 8 : 
                if(state.range(2) == 128) out128 = pppu::sub(ctx, p128, s128);
                else if(state.range(2) == 64) out64 = pppu::sub(ctx, p64, s64);
                state.SetLabel("sub_ps");
                break;
            case 9 : 
                if(state.range(2) == 128) out128 = pppu::sub(ctx, s128, s128);
                else if(state.range(2) == 64) out64 = pppu::sub(ctx, s64, s64);
                state.SetLabel("sub_ss");
                break;
            case 10 : 
                if(state.range(2) == 128) out128 = pppu::mul(ctx, p128, p128);
                else if(state.range(2) == 64) out64 = pppu::mul(ctx, p64, p64);
                state.SetLabel("mul_pp");
                break;
            case 11 : 
                if(state.range(2) == 128) out128 = pppu::mul(ctx, p128, s128);
                else if(state.range(2) == 64) out64 = pppu::mul(ctx, p64, s64);
                state.SetLabel("mul_ps");
                break;
            case 12 : 
                if(state.range(2) == 128) out128 = pppu::mul(ctx, s128, s128);
                else if(state.range(2) == 64) out64 = pppu::mul(ctx, s64, s64);
                state.SetLabel("mul_ss");
                break;
            case 13 : 
                if(state.range(2) == 128) out128 = pppu::msb(ctx, p128);
                else if(state.range(2) == 64) out64 = pppu::msb(ctx, p64);
                state.SetLabel("msb_p");
                break;
            case 14 : 
                if(state.range(2) == 128) out128 = pppu::msb(ctx, s128);
                else if(state.range(2) == 64) out64 = pppu::msb(ctx, s64);
                state.SetLabel("msb_s");
                break;
            case 15 : 
                if(state.range(2) == 128) out128 = pppu::eqz(ctx, p128);
                else if(state.range(2) == 64) out64 = pppu::eqz(ctx, p64);
                state.SetLabel("eqz_p");
                break;
            case 16 : 
                if(state.range(2) == 128) out128 = pppu::eqz(ctx, s128);
                else if(state.range(2) == 64) out64 = pppu::eqz(ctx, s64);
                state.SetLabel("eqz_s");
                break;
            case 17 : 
                if(state.range(2) == 128) out128 = pppu::sign(ctx, p128);
                else if(state.range(2) == 64) out64 = pppu::sign(ctx, p64);
                state.SetLabel("sign_p");
                break;
            case 18 : 
                if(state.range(2) == 128) out128 = pppu::sign(ctx, s128);
                else if(state.range(2) == 64) out64 = pppu::sign(ctx, s64);
                state.SetLabel("sign_s");
                break;
            case 19 : 
                if(state.range(2) == 128) out128 = pppu::abs(ctx, p128);
                else if(state.range(2) == 64) out64 = pppu::abs(ctx, p64);
                state.SetLabel("abs_p");
                break;
            case 20 : 
                if(state.range(2) == 128) out128 = pppu::abs(ctx, s128);
                else if(state.range(2) == 64) out64 = pppu::abs(ctx, s64);
                state.SetLabel("abs_s");
                break;
            case 21 : 
                if(state.range(2) == 128) out128 = pppu::square(ctx, p128);
                else if(state.range(2) == 64) out64 = pppu::square(ctx, p64);
                state.SetLabel("square_p");
                break;
            case 22 : 
                if(state.range(2) == 128) out128 = pppu::square(ctx, s128);
                else if(state.range(2) == 64) out64 = pppu::square(ctx, s64);
                state.SetLabel("square_s");
                break;
            case 23 : 
                if(state.range(2) == 128) out128 = pppu::r_lshift(ctx, fp128, shift);
                else if(state.range(2) == 64) out64 = pppu::r_lshift(ctx, fp64, shift);
                state.SetLabel("lshift_p");
                break;
            case 24 : 
                if(state.range(2) == 128) out128 = pppu::r_lshift(ctx, fs128, shift);
                else if(state.range(2) == 64) out64 = pppu::r_lshift(ctx, fs64, shift);
                state.SetLabel("lshift_s");
                break;
            case 25 : 
                if(state.range(2) == 128) out128 = pppu::r_trunc(ctx, fp128, shift);
                else if(state.range(2) == 64) out64 = pppu::r_trunc(ctx, fp64, shift);
                state.SetLabel("trunc_p");
                break;
            case 26 : 
                if(state.range(2) == 128) out128 = pppu::r_trunc(ctx, fs128, shift);
                else if(state.range(2) == 64) out64 = pppu::r_trunc(ctx, fs64, shift);
                state.SetLabel("trunc_s");
                break;
            case 27 : 
                if(state.range(2) == 128) outs128 = pppu::bitdec(ctx, p128, 128);
                else if(state.range(2) == 64) outs64 = pppu::bitdec(ctx, p64, 64);
                state.SetLabel("bitdec_p");
                break;
            case 28 : 
                if(state.range(2) == 128) outs128 = pppu::bitdec(ctx, s128, 128);
                else if(state.range(2) == 64) outs64 = pppu::bitdec(ctx, s64, 64);
                state.SetLabel("bitdec_s");
                break;
            case 29 : 
                if(state.range(2) == 128) outs128 = pppu::h1bitdec(ctx, p128, 128);
                else if(state.range(2) == 64) outs64 = pppu::h1bitdec(ctx, p64, 64);
                state.SetLabel("h1bitdec_p");
                break;
            case 30 : 
                if(state.range(2) == 128) outs128 = pppu::h1bitdec(ctx, s128, 128);
                else if(state.range(2) == 64) outs64 = pppu::h1bitdec(ctx, s64, 64);
                state.SetLabel("h1bitdec_s");
                break;
            case 31 : 
                if(state.range(2) == 128) out128 = pppu::bitcomp(ctx, pb128, 128);
                else if(state.range(2) == 64) out64 = pppu::bitcomp(ctx, pb64, 64);
                state.SetLabel("bitcomp_p");
                break;
            case 32 : 
                if(state.range(2) == 128) out128 = pppu::bitcomp(ctx, sb128, 128);
                else if(state.range(2) == 64) out64 = pppu::bitcomp(ctx, sb64, 64);
                state.SetLabel("bitcomp_s");
                break;
            case 33 : 
                if(state.range(2) == 128) out128 = pppu::div(ctx, p128, p128);
                else if(state.range(2) == 64) out64 = pppu::div(ctx, p64, p64);
                state.SetLabel("div_p");
                break;
            case 34 : 
                if(state.range(2) == 128) out128 = pppu::div(ctx, s128, s128);
                else if(state.range(2) == 64) out64 = pppu::div(ctx, s64, s64);
                state.SetLabel("div_s");
                break;
            case 35 : 
                if(state.range(2) == 128) out128 = pppu::sum(ctx, p128);
                else if(state.range(2) == 64) out64 = pppu::sum(ctx, p64);
                state.SetLabel("sum_p");
                break;
            case 36 : 
                if(state.range(2) == 128) out128 = pppu::sum(ctx, s128);
                else if(state.range(2) == 64) out64 = pppu::sum(ctx, s64);
                state.SetLabel("sum_s");
                break;
            case 37 : 
                if(state.range(2) == 128) out128 = pppu::matmul(ctx, pamat128, pbmat128);
                else if(state.range(2) == 64) out64 = pppu::matmul(ctx, pamat64, pbmat64);
                state.SetLabel("matmul_pp");
                break;
            case 38 : 
                if(state.range(2) == 128) out128 = pppu::matmul(ctx, pamat128, sbmat128);
                else if(state.range(2) == 64) out64 = pppu::matmul(ctx, pamat64, sbmat64);
                state.SetLabel("matmul_ps");
                break;
            case 39 : 
                if(state.range(2) == 128) out128 = pppu::matmul(ctx, samat128, pbmat128);
                else if(state.range(2) == 64) out64 = pppu::matmul(ctx, samat64, pbmat64);
                state.SetLabel("matmul_sp");
                break;
            case 40 : 
                if(state.range(2) == 128) out128 = pppu::matmul(ctx, samat128, sbmat128);
                else if(state.range(2) == 64) out64 = pppu::matmul(ctx, samat64, sbmat64);
                state.SetLabel("matmul_ss");
                break;
            default :
                break;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
        auto send = ctx->netio_p()->get_statistics().bytes_send;
        long long send_total = 0;
        for(int i = 0; i < send.size(); i++) {
            send_total += send[i];
        }
        auto tmp = prev;
        prev = send_total;
        send_total -= tmp;
        state.counters["comm"] = send_total;
    }
}

int main(int argc, char** argv) {
    fmt::print("");
    init(argc, argv);
    auto context = run_player(pid, num_parties);
    ctx = context.get();
    prepare_value(ctx, sizes);
    prev = 0;
    fmt::print("{}\n",pid);
    benchmark::RegisterBenchmark("BM_Semi2k", &BM_Test)->ArgsProduct({sizes, ops, bits})->UseManualTime()->MeasureProcessCPUTime()->Unit(benchmark::kMillisecond)->Iterations(iterations);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
