#pragma once

#include <span>
#include <set>
#include <unordered_set>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_set>

#include "../ndarray/array_ref.hpp"
#include "serializer.h"
#include "deserializer.h"

// concept HasFunctionSerialize<int[5]> = false
// template <typename T, size_t N>
// void serialize(Serializer& s, T arr[N]) {
//     s << std::span<const T, N>(arr);
// }

// HasFunctionSerializable<int[5]> = true
// but size of the array is inaccessable
// template <typename T>
// void serialize(Serializer& s, T arr[]) {
//     s << std::span<const T>(arr, ???);
// }

/************************ std::string ************************/

/// @brief Serializer for string.
/// @param sr Serializer reference
/// @param array String const reference
static void serialize(Serializer& sr, const std::string& str) {
    sr << str.length();
    sr << std::span(str.begin(), str.end());
}

/// @brief Deserializer for string.
/// @param sr Deserializer reference
/// @return Output string
static void deserialize(Deserializer& dr, std::string& str) {
    str.resize( dr.get<std::size_t>() );
    dr >> std::span(str.begin(), str.end());
}

/************************ std::array  ************************/

/// @brief Serializer for array.
/// @param sr Serializer reference
/// @param array Array const reference
template <typename T, size_t N>
void serialize(Serializer& sr, std::array<T, N> const& arr) {
    sr << std::span<const T, N>(arr);
}

/// @brief Deserializer for array.
/// @param sr Deserializer reference
/// @return Output array
template <typename T, size_t N>
void deserialize(Deserializer& dr, std::array<T, N>& arr) {
    dr >> std::span<T, N>(arr);
}

/************************ std::vector ************************/

/// @brief Serializer for vector.
/// @param sr Serializer reference
/// @param array Vector const reference
template <typename T>
void serialize(Serializer& sr, std::vector<T> const& vec) {
    sr << vec.size();
    sr << std::span(vec.begin(), vec.end());
}

/// @brief Deserializer for vector.
/// @param sr Deserializer reference
/// @return Output vector
template <typename T>
void deserialize(Deserializer& dr, std::vector<T>& vec) {
    vec.resize( dr.get<std::size_t>() );
    dr >> std::span(vec.begin(), vec.end());
}

/************************ std::queue ************************/

/************************ std::stack ************************/

/************************ std::list ************************/

/************************ std::map ************************/

/// @brief Serializer for map.
/// @param sr Serializer reference
/// @param array Map const reference
template <typename T1, typename T2>
void serialize(Serializer& sr, std::map<T1,T2> const& _map) {
    sr << _map.size();
    for(auto &item: _map) {
        sr << item;
    }
}

/// @brief Deserializer for map.
/// @param sr Deserializer reference
/// @return Output map
template <typename T1, typename T2>
void deserialize(Deserializer& dr, std::map<T1,T2>& _map) {
    int len = dr.get<std::size_t>();
    for(int index=0;index<len;index++) {
        _map.insert(dr.get<std::pair<T1,T2>>());
    }
}

/************************ std::set ************************/

/// @brief Serializer for set.
/// @param sr Serializer reference
/// @param array Set const reference
template <typename T>
void serialize(Serializer& sr, std::set<T> const& _set) {
    sr << _set.size();
    for(auto item: _set) {
        sr << item;
    }
}

/// @brief Deserializer for set.
/// @param sr Deserializer reference
/// @return Output set
template <typename T>
void deserialize(Deserializer& dr, std::set<T>& _set) {
    int len = dr.get<std::size_t>();
    for(int index=0;index<len;index++) {
        _set.insert(dr.get<T>());
    }
}

/// @brief Serializer for unordered set.
/// @param sr Serializer reference
/// @param array Unordered set const reference
template <typename T>
void serialize(Serializer& sr, std::unordered_set<T> const& _set) {
    sr << _set.size();
    for(auto &item: _set) {
        sr << item;
    }
}

/// @brief Deserializer for unordered set.
/// @param sr Deserializer reference
/// @return Output unordered set
template <typename T>
void deserialize(Deserializer& dr, std::unordered_set<T>& _set) {
    int len = dr.get<std::size_t>();
    for(int index=0;index<len;index++) {
        _set.insert(dr.get<T>());
    }
}

/************************ std::pair ************************/

/// @brief Serializer for pair.
/// @param sr Serializer reference
/// @param array Pair const reference
template <typename T1, typename T2>
void serialize(Serializer& sr, std::pair<T1,T2> const& piir) {
    sr << piir.first;
    sr << piir.second;
}

/// @brief Deserializer for pair.
/// @param sr Deserializer reference
/// @return Output pair
template <typename T1, typename T2>
void deserialize(Deserializer& dr, std::pair<T1,T2>& piir) {
    dr >> piir.first;
    dr >> piir.second;
}

/************************ std::tuple ************************/

/// @brief Implementation of serializer for tuple.
/// @param sr Serializer reference
/// @param array Tuple const reference
template<typename Tuple, std::size_t... Is>
void serialize_tuple_impl(Serializer& sr, const Tuple& t, std::index_sequence<Is...>)
{
    ((sr << std::get<Is>(t)), ...);
}

/// @brief Serializer for tuple.
/// @param sr Serializer reference
/// @param array Tuple const reference
template <typename ...T>
void serialize(Serializer& sr, std::tuple<T...> const& _tuple) {
    serialize_tuple_impl(sr, _tuple, std::index_sequence_for<T...>{});
}

/// @brief Implementation of deserializer for tuple.
/// @param sr Deserializer reference
/// @return Output tuple
template<typename Tuple, std::size_t... Is>
void deserialize_tuple_impl(Deserializer& dr,Tuple& t, std::index_sequence<Is...>)
{
    ((dr >> std::get<Is>(t)), ...);
}

/// @brief Deserializer for tuple.
/// @param sr Deserializer reference
/// @return Output tuple
template <typename ...T>
void deserialize(Deserializer& dr, std::tuple<T...>& _tuple) {
    deserialize_tuple_impl(dr, _tuple, std::index_sequence_for<T...>{});
}

/************************ core::ArrayRef ************************/

/// @brief Serializer for ArrayRef.
/// @param sr Serializer reference
/// @param array ArrayRef const reference
template <typename T>
void serialize(Serializer& sr, const core::ArrayRef<T>& arr) {
    std::vector<T> vec(arr.data(), arr.data() + arr.numel());
    sr << vec;
}

/// @brief Deserializer for ArrayRef.
/// @param sr Deserializer reference
/// @return Output ArrayRef
template <typename T>
void deserialize(Deserializer& dr, core::ArrayRef<T>& arr) {
    std::vector<T> vec;
    dr >> vec;
    arr = core::make_array(vec);
}
