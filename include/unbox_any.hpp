// 
// Copyright 2021 Clemens Cords
// Created on 21.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

#include <type_traits>

namespace jlwrap
{
    template<typename T>
    using is_primitive = typename std::conditional<
            std::is_same_v<T, bool> or
            std::is_same_v<T, char> or
            std::is_same_v<T, int8_t> or
            std::is_same_v<T, uint8_t> or
            std::is_same_v<T, int16_t> or
            std::is_same_v<T, uint16_t> or
            std::is_same_v<T, int32_t> or
            std::is_same_v<T, uint32_t> or
            std::is_same_v<T, int64_t> or
            std::is_same_v<T, uint64_t> or
            std::is_same_v<T, float> or
            std::is_same_v<T, double> or
            std::is_same_v<T, void> or
            std::is_same_v<T, void*> or
            std::is_same_v<T, nullptr_t>,
                    std::true_type,
                    std::false_type>::type;

    /// @brief unbox to primitive
    template<typename T, std::enable_if_t<is_primitive<T>::value, bool> = true>
    T unbox(jl_value_t* value)
    {
        if (jl_isa(value, jl_bool_type))
        {
            auto temp = jl_unbox_bool(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_char_type))
        {
            auto temp = jl_unbox_int8(value);
            return static_cast<T>(static_cast<char>(temp));
        }
        else if (jl_isa(value, jl_int8_type))
        {
            auto temp = jl_unbox_int8(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_int16_type))
        {
            auto temp = jl_unbox_int16(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_int32_type))
        {
            auto temp = jl_unbox_int32(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_int64_type))
        {
            auto temp = jl_unbox_int64(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_uint8_type))
        {
            auto temp = jl_unbox_uint8(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_uint16_type))
        {
            auto temp = jl_unbox_uint16(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_uint32_type))
        {
            auto temp = jl_unbox_uint32(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_uint64_type))
        {
            auto temp = jl_unbox_int64(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_float16_type))
        {
            auto temp = jl_unbox_float16(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_float32_type))
        {
            auto temp = jl_unbox_float32(value);
            return static_cast<T>(temp);
        }
        else if (jl_isa(value, jl_float64_type))
        {
            auto temp = jl_unbox_float64(value);
            return static_cast<T>(temp);
        }
    }

    /// @brief unbox to string
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, bool> = true>
    T unbox(jl_value_t* value)
    {
        if (jl_isa(value, jl_string_type))
            return std::string(jl_string_data(value));

        static jl_function_t* is_method_available = State::script("return jlwrap.is_method_available");
        static jl_function_t* to_string = jl_get_function(jl_main_module, "string");

        if (not jl_unbox_bool(jl_call2(is_method_available, (jl_value_t*) to_string, value)))
        {
            char* type_name = jl_string_data(jl_call1(to_string, jl_typeof(value)));
            throw MethodException("string", {type_name});
        }

        return std::string(jl_string_data(jl_call1(to_string, value)));
    }
}