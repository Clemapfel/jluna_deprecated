// 
// Copyright 2021 Clemens Cords
// Created on 21.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <.src/julia_extension.h>
#include <string>
#include <type_traits>

namespace jluna
{
    /// @brief unbox identity
    jl_value_t* unbox(jl_value_t* value)
    {
        return value;
    }

    /// @brief unbox to bool
    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_bool_type))
                return jl_unbox_bool(value);
        else
            return jl_unbox_bool(jl_call2(convert, (jl_value_t*) jl_bool_type, value));
    }

    /// @brief unbox to char
    template<typename T, std::enable_if_t<std::is_same_v<T, char>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_bool_type))
                return jl_unbox_int8(value);
        else
            return jl_unbox_int8(jl_call2(convert, (jl_value_t*) jl_char_type, value));
    }

    /// @brief unbox to int8
    template<typename T, std::enable_if_t<std::is_same_v<T, int8_t>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_int8_type))
                return jl_unbox_int8(value);
        else
            return jl_unbox_int8(jl_call2(convert, (jl_value_t*) jl_int8_type, value));
    }

    /// @brief unbox to int16
    template<typename T, std::enable_if_t<std::is_same_v<T, int16_t>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_int16_type))
                return jl_unbox_int16(value);
        else
            return jl_unbox_int16(jl_call2(convert, (jl_value_t*) jl_int16_type, value));
    }

    /// @brief unbox to int32
    template<typename T, std::enable_if_t<std::is_same_v<T, int32_t>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_int32_type))
                return jl_unbox_int32(value);
        else
            return jl_unbox_int32(jl_call2(convert, (jl_value_t*) jl_int32_type, value));
    }

    /// @brief unbox to int64
    template<typename T, std::enable_if_t<std::is_same_v<T, int64_t>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_int64_type))
                return jl_unbox_int64(value);
        else
            return jl_unbox_int64(jl_call2(convert, (jl_value_t*) jl_int64_type, value));
    }

    /// @brief unbox to uint8
    template<typename T, std::enable_if_t<std::is_same_v<T, uint8_t>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint8_type))
                return jl_unbox_uint8(value);
        else
            return jl_unbox_uint8(jl_call2(convert, (jl_value_t*) jl_uint8_type, value));
    }

    /// @brief unbox to uint16
    template<typename T, std::enable_if_t<std::is_same_v<T, uint16_t>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint16_type))
                return jl_unbox_uint16(value);
        else
            return jl_unbox_uint16(jl_call2(convert, (jl_value_t*) jl_uint16_type, value));
    }

    /// @brief unbox to uint32
    template<typename T, std::enable_if_t<std::is_same_v<T, uint32_t>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint32_type))
                return jl_unbox_uint32(value);
        else
            return jl_unbox_uint32(jl_call2(convert, (jl_value_t*) jl_uint32_type, value));
    }

    /// @brief unbox to uint64
    template<typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint64_type))
                return jl_unbox_uint64(value);
        else
            return jl_unbox_uint64(jl_call2(convert, (jl_value_t*) jl_int64_type, value));
    }

    /// @brief unbox to float16 & float32
    template<typename T, std::enable_if_t<std::is_same_v<T, float>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_float16_type))
                return jl_unbox_float16(value);
        else if (jl_isa(value, (jl_value_t*) jl_float32_type))
            return jl_unbox_float32(value);
        else
            return jl_unbox_float32(jl_call2(convert, (jl_value_t*) jl_float32_type, value));
    }

    /// @brief unbox to float64
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_float64_type))
                return jl_unbox_float64(value);
        else
            return jl_unbox_float64(jl_call2(convert, (jl_value_t*) jl_float64_type, value));
    }

    /// @brief unbox to string
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, bool> = true>
    T unbox(jl_value_t* value)
    {
        if (jl_isa(value, (jl_value_t*) jl_string_type))
            return std::string(jl_string_data(value));

        static jl_function_t* is_method_available = jl_eval_string("return jluna.is_method_available");
        static jl_function_t* to_string = jl_get_function(jl_main_module, "string");

        if (not jl_unbox_bool(jl_call2(is_method_available, (jl_value_t*) to_string, value)))
            assert(false);

        return std::string(jl_string_data(jl_call1(to_string, value)));
    }

    /// @brief unbox to complex
    template<typename T, typename S = typename T::value_type, std::enable_if_t<std::is_same_v<T, std::complex<S>>, bool> = true>
    T unbox(jl_value_t* value)
    {
        auto* re = jl_get_nth_field(value, 0);
        auto* im = jl_get_nth_field(value, 1);

        return std::complex<S>(unbox<S>(re), unbox<S>(im));
    }

    /// @brief unbox to pair
    template<typename T,
        typename T1 = typename T::first_type,
        typename T2 = typename T::second_type,
        std::enable_if_t<std::is_same_v<T, std::pair<T1, T2>>, bool> = true>
    T unbox(jl_value_t* value)
    {
        auto* first = jl_get_nth_field(value, 0);
        auto* second = jl_get_nth_field(value, 1);

        return std::pair<T1, T2>(unbox<T1>(first), unbox<T2>(second));
    }
}