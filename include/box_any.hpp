/// @brief box to 
/// @brief box to Copyright 2021 Clemens Cords
/// @brief box to Created on 24.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <type_traits>
#include <string>
#include <set>

namespace jluna
{
    /// @brief box to identity
    jl_value_t* box(jl_value_t* value);

    /// @brief box to cast first
    template<Decayable T>
    jl_value_t* box(T&& value);

    /// @brief box to string
    jl_value_t* box(const std::string& value);

    /// @brief box to c string
    jl_value_t* box(const char* value);

    /// @brief box to bool
    jl_value_t* box(bool value);

    /// @brief box to char
    jl_value_t* box(char value);

    /// @brief box to int8
    jl_value_t* box(int8_t value);

    /// @brief box to int16
    jl_value_t* box(int16_t value);

    /// @brief box to int32
    jl_value_t* box(int32_t value);

    /// @brief box to int64
    jl_value_t* box(int64_t value);

    /// @brief box to uint8
    jl_value_t* box(uint8_t value);

    /// @brief box to uint16
    jl_value_t* box(uint16_t value);

    /// @brief box to uint32
    jl_value_t* box(uint32_t value);

    /// @brief box to uint64
    jl_value_t* box(uint64_t value);

    /// @brief box to float
    jl_value_t* box(float value);

    /// @brief box to double
    jl_value_t* box(double value);

    /// @brief box to complex
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    jl_value_t* box(std::complex<T> value);

    /// @brief box to vector
    template<typename T>
    jl_value_t* box(const std::vector<T>& value);

    /// @brief box to pair
    template<typename T1, typename T2>
    jl_value_t* box(const std::pair<T1, T2>& value);

    /// @brief box tuple
    template<typename... Ts>
    jl_value_t* box(const std::tuple<Ts...>& value);

    /// @brief box map, unordered map to IdMap
    template<IsDict T, typename Key_t = typename T::key_type, typename Value_t = typename T::mapped_type>
    jl_value_t* box(const T& value);

    /// @brief box set
    template<typename T>
    jl_value_t* box(const std::set<T>& value);

    /// @brief box to explicit return type
    template<typename Return_t, CastableTo<Return_t> Arg_t>
    jl_value_t* box(Arg_t t);
}

#include ".src/box_any.inl"