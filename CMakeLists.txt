# Cmake Settings
cmake_minimum_required(VERSION 3.10)

# C++ Settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "/usr/bin/g++-13")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive --std=c++2a -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

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
add_library(PPPU SHARED ${CONFIG} ${CONTEXT} ${DATATYPES} ${MPC} ${NDARRAY} ${NETWORK} ${TOOLS})
add_library(PPPUExample SHARED ${EXAMPLE})

# Examples
add_executable(tutorial src/example/tutorials/tutorial.cpp)
target_link_libraries(tutorial PPPU PPPUExample gmp gmpxx ssl crypto pthread relic)

# add_executable(BENCHMARK_CONTEXT "src/example/benchmark/context_benchmark.cc")
# target_link_libraries(BENCHMARK_CONTEXT PPPU PPPUExample gmp gmpxx ssl crypto pthread benchmark::benchmark benchmark::benchmark_main)

# add_executable(TEST_CONTEXT_BASIC "src/example/unittest/context_basic_test.cc")
# target_link_libraries(TEST_CONTEXT_BASIC PPPU PPPUExample gmp gmpxx ssl crypto pthread GTest::gtest_main)
# include(GoogleTest)
# gtest_add_tests(TARGET TEST_CONTEXT_BASIC)

# add_executable(TEST_MPC_SEMI2K "src/example/unittest/mpc_semi2k_test.cc")
# target_link_libraries(TEST_MPC_SEMI2K PPPU PPPUExample gmp gmpxx ssl crypto pthread GTest::gtest_main)
# include(GoogleTest)
# gtest_add_tests(TARGET TEST_MPC_SEMI2K)

# add_executable(TEST_NETWORK "src/example/unittest/network_test.cc")
# target_link_libraries(TEST_NETWORK PPPU PPPUExample gmp gmpxx ssl crypto pthread GTest::gtest_main)
# include(GoogleTest)
# gtest_add_tests(TARGET TEST_NETWORK)

# add_executable(TEST_CONTEXT_MATH "src/example/unittest/context_math_test.cc")
# target_link_libraries(TEST_CONTEXT_MATH PPPU PPPUExample gmp gmpxx ssl crypto pthread gtest)
# include(GoogleTest)
# gtest_add_tests(TARGET TEST_CONTEXT_MATH)

# add_executable(TEST_CONTEXT_COMPARE "src/example/unittest/context_compare_test.cc")
# target_link_libraries(TEST_CONTEXT_COMPARE pppu example gmp gmpxx ssl crypto pthread gtest)
# include(GoogleTest)
# gtest_add_tests(TARGET TEST_CONTEXT_COMPARE)

# Install
install(TARGETS PPPU LIBRARY DESTINATION lib)
install(TARGETS PPPUExample LIBRARY DESTINATION lib)
install(TARGETS tutorial RUNTIME DESTINATION bin)
install(FILES src/config/config.h DESTINATION include/config)
install(FILES src/context/value.h DESTINATION include/context)
install(FILES src/context/visibility.h DESTINATION include/context)
install(FILES src/context/basic/basic.h DESTINATION include/context/basic)
install(FILES src/context/basic/factory.h DESTINATION include/context/basic)
install(FILES src/context/basic/fxp.h DESTINATION include/context/basic)
install(FILES src/context/basic/primitive.h DESTINATION include/context/basic)
install(FILES src/context/basic/prot_wrapper.h DESTINATION include/context/basic)
install(FILES src/context/basic/raw.h DESTINATION include/context/basic)
install(FILES src/context/basic/util.h DESTINATION include/context/basic)
install(FILES src/context/compare/compare.h DESTINATION include/context/compare)
install(FILES src/context/math/div.h DESTINATION include/context/math)
install(FILES src/context/math/exp.h DESTINATION include/context/math)
install(FILES src/context/math/log.h DESTINATION include/context/math)
install(FILES src/context/math/polynomial.h DESTINATION include/context/math)
install(FILES src/context/math/pow.h DESTINATION include/context/math)
install(FILES src/context/math/round.h DESTINATION include/context/math)
install(FILES src/context/math/sigmoid.h DESTINATION include/context/math)
install(FILES src/context/math/sqrt.h DESTINATION include/context/math)
install(FILES src/context/math/tools.h DESTINATION include/context/math)
install(FILES src/context/shape/concatenate.h DESTINATION include/context/shape)
install(FILES src/context/shape/reduce.h DESTINATION include/context/shape)
install(FILES src/context/shape/sort.h DESTINATION include/context/shape)
install(FILES src/context/shape/tools.h DESTINATION include/context/shape)
install(FILES src/datatypes/concepts.h DESTINATION include/datatypes)
install(FILES src/datatypes/eigen.h DESTINATION include/datatypes)
install(FILES src/datatypes/fixed_point.h DESTINATION include/datatypes)
install(FILES src/datatypes/int128.h DESTINATION include/datatypes)
install(FILES src/datatypes/mpx2k.h DESTINATION include/datatypes)
install(FILES src/datatypes/mpxp.h DESTINATION include/datatypes)
install(FILES src/datatypes/Z2k.h DESTINATION include/datatypes)
install(FILES src/datatypes/Zp.h DESTINATION include/datatypes)
install(FILES src/mpc/random_generator.h DESTINATION include/mpc)
install(FILES src/ndarray/array_ref.h DESTINATION include/ndarray)
install(FILES src/ndarray/concatenate.h DESTINATION include/ndarray)
install(FILES src/ndarray/ndarray_ref.h DESTINATION include/ndarray)
install(FILES src/ndarray/operations.h DESTINATION include/ndarray)
install(FILES src/ndarray/tools.h DESTINATION include/ndarray)
install(FILES src/network/bitrate.h DESTINATION include/network)
install(FILES src/network/comm_package.h DESTINATION include/network)
install(FILES src/network/futures.h DESTINATION include/network)
install(FILES src/network/mp_connect.h DESTINATION include/network)
install(FILES src/network/multi_party_player.h DESTINATION include/network)
install(FILES src/network/playerid.h DESTINATION include/network)
install(FILES src/network/socket_package.h DESTINATION include/network)
install(FILES src/network/statistics.h DESTINATION include/network)
install(FILES src/network/two_party_player.h DESTINATION include/network)
install(FILES src/serialization/concepts.h DESTINATION include/serialization)
install(FILES src/serialization/deserializer_impl.h DESTINATION include/serialization)
install(FILES src/serialization/deserializer.h DESTINATION include/serialization)
install(FILES src/serialization/exceptions.h DESTINATION include/serialization)
install(FILES src/serialization/serializer_impl.h DESTINATION include/serialization)
install(FILES src/serialization/serializer.h DESTINATION include/serialization)
install(FILES src/serialization/stl.h DESTINATION include/serialization)
install(FILES src/tools/bit_reference.h DESTINATION include/tools)
install(FILES src/tools/bit_vector.h DESTINATION include/tools)
install(FILES src/tools/byte_vector.h DESTINATION include/tools)
install(FILES src/tools/expr_template.h DESTINATION include/tools)
install(FILES src/tools/math.h DESTINATION include/tools)
install(FILES src/tools/raw_vector.h DESTINATION include/tools)
install(FILES src/tools/timer.h DESTINATION include/tools)
