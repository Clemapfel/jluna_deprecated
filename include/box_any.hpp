/// @brief box to 
/// @brief box to Copyright 2021 Clemens Cords
/// @brief box to Created on 24.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <type_traits>
#include <string>
#include <.src/common.hpp>

namespace jluna
{
    /// @brief box to identity
    jl_value_t* box(jl_value_t* value)
    {
        return value;
    }

    /// @brief box to cast first
    template<Decayable T>
    jl_value_t* box(T&& value)
    {
        return (jl_value_t*) (std::forward<T*>(value));
    }

    /// @brief box to string
    jl_value_t* box(const std::string& value)
    {
        std::string command = "return \"" + value + "\"";
        return jl_eval_string(command.c_str());
    }

    /// @brief box to c string
    jl_value_t* box(const char* value)
    {
        std::string command = "return \"" + std::string(value) + "\"";
        return jl_eval_string(command.c_str());
    }

    /// @brief box to bool
    jl_value_t* box(bool value)
    {
        return jl_box_bool(value);
    }

    /// @brief box to char
    jl_value_t* box(char value)
    {
        return jl_box_char(value);
    }

    /// @brief box to int8
    jl_value_t* box(int8_t value)
    {
        return jl_box_int8(value);
    }

    /// @brief box to int16
    jl_value_t* box(int16_t value)
    {
        return jl_box_int16(value);
    }

    /// @brief box to int32
    jl_value_t* box(int32_t value)
    {
        return jl_box_int32(value);
    }

    /// @brief box to int64
    jl_value_t* box(int64_t value)
    {
        return jl_box_int64(value);
    }

    /// @brief box to uint8
    jl_value_t* box(uint8_t value)
    {
        return jl_box_uint8(value);
    }

    /// @brief box to uint16
    jl_value_t* box(uint16_t value)
    {
        return jl_box_uint16(value);
    }

    /// @brief box to uint32
    jl_value_t* box(uint32_t value)
    {
        return jl_box_uint32(value);
    }

    /// @brief box to uint64
    jl_value_t* box(uint64_t value)
    {
        return jl_box_uint64(value);
    }

    /// @brief box to float
    jl_value_t* box(float value)
    {
        return jl_box_float32(value);
    }

    /// @brief box to double
    jl_value_t* box(double value)
    {
        return jl_box_float64(value);
    }

    /// @brief box to complex
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    jl_value_t* box(std::complex<T> value)
    {
        static jl_function_t* complex = jl_get_function(jl_base_module, "complex");
        return jl_call2(complex, box(value.real()), box(value.imag()));
    }

    /// @brief box to pair
    template<typename T1, typename T2>
    jl_value_t* box(std::pair<T1, T2> value)
    {
        static jl_function_t* pair = jl_get_function(jl_core_module, "Pair");
        return jl_call2(pair, box(value.first), box(value.second));
    }

    /// @brief box range to vector
    template<Iterable Range_t>
    jl_value_t* box(Range_t value)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("jluna");
        static jl_function_t* vector = jl_get_function(jluna_module, "make_vector");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());
        for (auto& v : value)
            args.push_back(box(v));

        return jl_call(vector, args.data(), args.size());
    }

    /// @brief box tuple
    template<typename... Ts>
    jl_value_t* box(std::tuple<Ts...> value)
    {
        static jl_function_t* tuple = jl_get_function(jl_core_module, "tuple");

        std::vector<jl_value_t*> args;
        args.reserve(sizeof...(Ts));

        std::apply([&](auto&&... elements) {
            (args.push_back(box(elements)), ...);
        }, value);

        return jl_call(tuple, args.data(), args.size());
    }

    /// @brief box to explicit return type
    template<typename Return_t, CastableTo<Return_t> Arg_t>
    jl_value_t* box(Arg_t t)
    {
        return box(static_cast<Return_t>(t));
    }
}