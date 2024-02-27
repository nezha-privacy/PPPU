# Cmake Settings
cmake_minimum_required(VERSION 3.10)

# C++ Settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive --std=c++2a -w")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
# set(CMAKE_BUILD_TYPE "Debug")

# Project
project(
    PPPU
    VERSION 1.0
    LANGUAGES CXX C
    DESCRIPTION "")

include(CMakePrintHelpers)

# Boost
find_package(Boost 1.80 HINTS "/usr/local/include/boost") 
find_library(ProgramOptions boost_program_options /usr/local/lib)
message(STATUS "Boost version: ${Boost_VERSION}")

# Fmt
find_package(fmt HINTS "/usr/local/lib")
find_library(FMT_LIBRARY fmt HINTS "/usr/local/lib")
message(STATUS "fmt version: ${fmt_VERSION} ${FMT_LIBRARY}")

# Google Benchmark
# find_package(benchmark REQUIRED HINTS "/home/zh/PPPU/src/benchmark/build")

# Googletest
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY git@github.com:google/googletest.git
#   GIT_TAG release-1.12.1
# )
# FetchContent_MakeAvailable(googletest)
# include_directories("/home/zh/PPPU/build/_deps/googletest-src/googletest/include")
# enable_testing()

# Relic
# find_package(relic REQUIRED)

# include Directories
include_directories( 
    ${Boost_INCLUDE_DIRS}
    "/usr/local/include/fmt"
    "src")

# Files
file(GLOB CONFIG src/config/*.cpp)
file(GLOB CONTEXT src/context/**/*.cpp)
file(GLOB DATATYPES src/datatypes/*.cpp)
file(GLOB EXAMPLE src/example/*.cpp)
file(GLOB MPC src/mpc/*.cpp)
file(GLOB NDARRAY src/ndarray/*.cpp)
file(GLOB NETWORK src/network/*.cpp)
file(GLOB SERIALIZATION src/serialization/*.cpp)
file(GLOB TOOLS src/tools/*vector.cpp)

# Print Variables
cmake_print_variables(CONFIG)
cmake_print_variables(CONTEXT)
cmake_print_variables(DATATYPES)
cmake_print_variables(EXAMPLE)
cmake_print_variables(MPC)
cmake_print_variables(NDARRAY)
cmake_print_variables(NETWORK)
cmake_print_variables(SERIALIZATION)
cmake_print_variables(TOOLS)

# Add Libraries
add_library(config ${CONFIG})
add_library(context ${CONTEXT})
add_library(datatypes ${DATATYPES})
add_library(example ${EXAMPLE})
add_library(mpc ${MPC})
add_library(ndarray ${NDARRAY})
add_library(network ${NETWORK})
add_library(tools ${TOOLS})

# Examples
add_executable(tutorial src/example/tutorials/tutorial.cpp)
target_link_libraries(tutorial config context datatypes example mpc ndarray network tools gmp gmpxx ssl crypto fmt::fmt-header-only pthread)

# add_executable(BENCHMARK_CONTEXT "src/example/benchmark/context_benchmark.cc")
# target_link_libraries(BENCHMARK_CONTEXT config context datatypes example mpc ndarray network tools gmp gmpxx ssl crypto fmt::fmt-header-only pthread benchmark::benchmark benchmark::benchmark_main)

# add_executable(TEST_CONTEXT_BASIC "src/example/unittests/context_basic_test.cc")
# target_link_libraries(TEST_CONTEXT_BASIC config context datatypes example mpc ndarray network tools gmp gmpxx ssl crypto fmt::fmt-header-only pthread GTest::gtest_main)
# include(GoogleTest)
# gtest_add_tests(TARGET TEST_CONTEXT_BASIC)

# add_executable(TEST_MPC_SEMI2K "src/example/unittests/mpc_semi2k_test.cc")
# target_link_libraries(TEST_MPC_SEMI2K config context datatypes example mpc ndarray network tools gmp gmpxx ssl crypto fmt::fmt-header-only pthread GTest::gtest_main)
# include(GoogleTest)
# gtest_add_tests(TARGET TEST_MPC_SEMI2K)

# add_executable(TEST_NETWORK "src/example/unittests/network_test.cc")
# target_link_libraries(TEST_NETWORK config context datatypes example mpc ndarray network tools gmp gmpxx ssl crypto fmt::fmt-header-only pthread GTest::gtest_main)
# include(GoogleTest)
# gtest_add_tests(TARGET TEST_NETWORK)
