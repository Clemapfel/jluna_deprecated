// 
// Copyright 2021 Clemens Cords
// Created on 24.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

#include <type_traits>
#include <string>

#include <unbox_any.hpp>

namespace jlwrap
{
    jl_value_t* box(jl_value_t* value)
    {
        return value;
    }

    template<typename T,
            std::enable_if_t<
                    detail::is_primitive<T>::value or
                    std::is_same_v<T, std::string>,
            bool> = true>
    jl_value_t* box(T value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (std::is_same_v<T, bool>)
        {
            return jl_box_bool(value);
        }
        else if (std::is_same_v<T, char>)
        {
            return jl_box_char(value);
        }
        else if (std::is_same_v<T, int8_t>)
        {
            return jl_box_int8(value);
        }
        else if (std::is_same_v<T, int16_t>)
        {
            return jl_box_int16(value);
        }
        else if (std::is_same_v<T, int32_t>)
        {
            return jl_box_int32(value);
        }
        else if (std::is_same_v<T, int64_t>)
        {
            return jl_box_int64(value);
        }
        else if (std::is_same_v<T, uint8_t>)
        {
            return jl_box_uint8(value);
        }
        else if (std::is_same_v<T, uint16_t>)
        {
            return jl_box_uint16(value);
        }
        else if (std::is_same_v<T, uint32_t>)
        {
            return jl_box_uint32(value);
        }
        else if (std::is_same_v<T, uint64_t>)
        {
            return jl_box_uint64(value);
            }
        else if (std::is_same_v<T, float>)
        {
            return jl_box_float32(value);
        }
        else if (std::is_same_v<T, double>)
        {
            return jl_box_float64(value);
        }
        else if (std::is_same_v<T, std::string>)
        {
            //return State::script("return " + std::to_string(value));
        }
        else
            return jl_box_voidpointer(nullptr); // placeholder
    }
}