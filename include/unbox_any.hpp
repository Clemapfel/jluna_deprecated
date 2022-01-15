// 
// Copyright 2021 Clemens Cords
// Created on 21.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <.src/julia_extension.h>
#include <string>
#include <vector>
#include <type_traits>
#include <utility>
#include <.src/common.hpp>

namespace jluna
{
    /// @brief unbox identity
    jl_value_t* unbox(jl_value_t* value);

    /// @brief unbox to bool
    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to char
    template<typename T, std::enable_if_t<std::is_same_v<T, char>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to int8
    template<typename T, std::enable_if_t<std::is_same_v<T, int8_t>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to int16
    template<typename T, std::enable_if_t<std::is_same_v<T, int16_t>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to int32
    template<typename T, std::enable_if_t<std::is_same_v<T, int32_t>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to int64
    template<typename T, std::enable_if_t<std::is_same_v<T, int64_t>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to uint8
    template<typename T, std::enable_if_t<std::is_same_v<T, uint8_t>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to uint16
    template<typename T, std::enable_if_t<std::is_same_v<T, uint16_t>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to uint32
    template<typename T, std::enable_if_t<std::is_same_v<T, uint32_t>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to uint64
    template<typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to float16 & float32
    template<typename T, std::enable_if_t<std::is_same_v<T, float>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to float64
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to string
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to complex
    template<typename T, typename S = typename T::value_type, std::enable_if_t<std::is_same_v<T, std::complex<S>>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to pair
    template<typename T,
        typename T1 = typename T::first_type,
        typename T2 = typename T::second_type,
        std::enable_if_t<std::is_same_v<T, std::pair<T1, T2>>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to vector
    template<typename T,
        typename U = typename T::value_type,
        std::enable_if_t<std::is_same_v<T, std::vector<U>>, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox to array
    template<typename T,
        size_t N = array_size<T>::value,
        typename U = typename T::value_type,
        std::enable_if_t<std::is_same_v<T, std::array<U, N>>, bool> = true>
    T unbox(jl_value_t* value);

    ///@brief unbox tuple (but not pair)
    template<IsTuple T, std::enable_if_t<std::tuple_size<T>::value != 2, bool> = true>
    T unbox(jl_value_t* value);

    /// @brief unbox map
    template<IsDict T, typename Key_t = typename T::key_type, typename Value_t = typename T::mapped_type>
    T unbox(jl_value_t* value);

    /// @brief unbox set
    template<typename T, typename U = typename T::value_type, std::enable_if_t<std::is_same_v<T, std::set<U>>, bool> = true>
    T unbox(jl_value_t* value);
}

#include ".src/unbox_any.inl"