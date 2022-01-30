// 
// Copyright 2022 Clemens Cords
// Created on 30.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <.src/common.hpp>
#include <type_traits>

namespace jluna
{
    /// @brief directly decay to jl_value_t*
    template<Decayable T>
    jl_value_t* box(const T& value);

    /// @brief box identity
    jl_value_t* box(jl_value_t* value);

    /// @brief box to explicit return type
    template<typename Return_t, CastableTo<Return_t> Arg_t>
    jl_value_t* box(Arg_t t);

    /// @brief box to c string
    template<typename T, std::enable_if_t<std::is_same_v<T, const char*>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to bool
    template<typename T, std::enable_if_t<std::is_same_v<T, std::bool_constant<true>>, bool> = true>
    jl_value_t* box(T);

    template<typename T, std::enable_if_t<std::is_same_v<T, std::bool_constant<false>>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to char
    template<typename T, std::enable_if_t<std::is_same_v<T, char>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to int8
    template<typename T, std::enable_if_t<std::is_same_v<T, int8_t>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to int16
    template<typename T, std::enable_if_t<std::is_same_v<T, int16_t>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to int32
    template<typename T, std::enable_if_t<std::is_same_v<T, int32_t>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to int64
    template<typename T, std::enable_if_t<std::is_same_v<T, int64_t>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to uint8
    template<typename T, std::enable_if_t<std::is_same_v<T, uint8_t>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to uint16
    template<typename T, std::enable_if_t<std::is_same_v<T, uint16_t>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to uint32
    template<typename T, std::enable_if_t<std::is_same_v<T, uint32_t>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to uint64
    template<typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to float
    template<typename T, std::enable_if_t<std::is_same_v<T, float>, bool> = true>
    jl_value_t* box(T);jl_value_t* box(float value);

    /// @brief box to double
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to string
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, bool> = true>
    jl_value_t* box(const T&);

    /// @brief box to complex
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    jl_value_t* box(std::complex<T> value);

    /// @brief box to vector
    template<typename T,
        typename U = typename T::value_type,
        std::enable_if_t<std::is_same_v<T, std::vector<U>>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to pair
    template<typename T,
        typename T1 = typename T::first_type,
        typename T2 = typename T::second_type,
        std::enable_if_t<std::is_same_v<T, std::pair<T1, T2>>, bool> = true>
    jl_value_t* box(T);

    /// @brief box to tuple
    template<IsTuple T, std::enable_if_t<std::tuple_size<T>::value != 2, bool> = true>
    jl_value_t* box(T);

    /// @brief box map to IdDict
    template<typename T,
        typename Key_t = typename T::key_type,
        typename Value_t = typename T::mapped_type,
        std::enable_if_t<std::is_same_v<T, std::map<Key_t, Value_t>>, bool> = true>
    jl_value_t* box(const T&);

    /// @brief box unordered map to Dict
    template<typename T,
        typename Key_t = typename T::key_type,
        typename Value_t = typename T::mapped_type,
        std::enable_if_t<std::is_same_v<T, std::unordered_map<Key_t, Value_t>>, bool> = true>
    jl_value_t* box(const T&);

    /// @brief box set
    template<typename T,
        typename U = typename T::value_type,
        std::enable_if_t<std::is_same_v<T, std::set<U>>, bool> = true>
    jl_value_t* box(const T&);
    
    /// @brief box lambda with signature () -> Any
    template<LambdaType<> T>
    jl_value_t* box(const T&);

    /// @brief box lambda with signature (Any) -> Any
    template<LambdaType<Any> T>
    jl_value_t* box(const T&);

    /// @brief box lambda with signature (Any, Any) -> Any
    template<LambdaType<Any, Any> T>
    jl_value_t* box(const T&);

    /// @brief box lambda with signature (Any, Any, Any) -> Any
    template<LambdaType<Any, Any, Any> T>
    jl_value_t* box(const T&);

    /// @brief box lambda with signature (Any, Any, Any, Any) -> Any
    template<LambdaType<Any, Any, Any, Any> T>
    jl_value_t* box(const T&);

    /// @brief box lambda with signature (vector{Any}) -> Any
    template<LambdaType<std::vector<Any>> T>
    jl_value_t* box(const T&);
}

#include ".src/box_any.inl"