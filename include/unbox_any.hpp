// 
// Copyright 2021 Clemens Cords
// Created on 21.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

#include <type_traits>

namespace jlwrap
{
    namespace detail
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
                std::is_same_v<T, double>,
                std::true_type,
                std::false_type>::type;
    }

    /// @brief unbox to primitive
    template<typename T, std::enable_if_t<detail::is_primitive<T>::value, bool> = true>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (std::is_same_v<T, bool>)
        {
            if (jl_isa(value, jl_bool_type))
                return jl_unbox_bool(value);
            else
                return jl_unbox_bool(jl_call2(convert, (jl_value_t*) jl_bool_type, value));
        }
        else if (std::is_same_v<T, char>)
        {
            if (jl_isa(value, jl_bool_type))
                return jl_unbox_int8(value);
            else
                return jl_unbox_int8(jl_call2(convert, (jl_value_t*) jl_char_type, value));
        }
        else if (std::is_same_v<T, int8_t>)
        {
            if (jl_isa(value, jl_int8_type))
                return jl_unbox_int8(value);
            else
                return jl_unbox_int8(jl_call2(convert, (jl_value_t*) jl_int8_type, value));
        }
        else if (std::is_same_v<T, int16_t>)
        {
            if (jl_isa(value, jl_int16_type))
                return jl_unbox_int16(value);
            else
                return jl_unbox_int16(jl_call2(convert, (jl_value_t*) jl_int16_type, value));
        }
        else if (std::is_same_v<T, int32_t>)
        {
            if (jl_isa(value, jl_int32_type))
                return jl_unbox_int32(value);
            else
                return jl_unbox_int32(jl_call2(convert, (jl_value_t*) jl_int32_type, value));
        }
        else if (std::is_same_v<T, int64_t>)
        {
            if (jl_isa(value, jl_int64_type))
                return jl_unbox_int64(value);
            else
                return jl_unbox_int64(jl_call2(convert, (jl_value_t*) jl_int64_type, value));
        }
        else if (std::is_same_v<T, uint8_t>)
        {
            if (jl_isa(value, jl_uint8_type))
                return jl_unbox_uint8(value);
            else
                return jl_unbox_uint8(jl_call2(convert, (jl_value_t*) jl_uint8_type, value));
        }
        else if (std::is_same_v<T, uint16_t>)
        {
            if (jl_isa(value, jl_uint16_type))
                return jl_unbox_uint16(value);
            else
                return jl_unbox_uint16(jl_call2(convert, (jl_value_t*) jl_uint16_type, value));
        }
        else if (std::is_same_v<T, uint32_t>)
        {
            if (jl_isa(value, jl_uint32_type))
                return jl_unbox_uint32(value);
            else
                return jl_unbox_uint32(jl_call2(convert, (jl_value_t*) jl_uint32_type, value));
        }
        else if (std::is_same_v<T, uint64_t>)
        {
            if (jl_isa(value, jl_uint64_type))
                return jl_unbox_uint64(value);
            else
                return jl_unbox_uint64(jl_call2(convert, (jl_value_t*) jl_int64_type, value));
        }
        else if (std::is_same_v<T, float>)
        {
            if (jl_isa(value, jl_float16_type))
                return jl_unbox_float16(value);
            else if (jl_isa(value, jl_float32_type))
                return jl_unbox_float32(value);
            else
                return jl_unbox_float32(jl_call2(convert, (jl_value_t*) jl_float32_type, value));
        }
        else if (std::is_same_v<T, double>)
        {
            if (jl_isa(value, jl_float64_type))
                return jl_unbox_float64(value);
            else
                return jl_unbox_float64(jl_call2(convert, (jl_value_t*) jl_float64_type, value));
        }
        else
            return T();  // unreachable code
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
            assert(false);

        return std::string(jl_string_data(jl_call1(to_string, value)));
    }

    /// @brief unbox to vector
    template<typename T,
        typename Value_t = typename T::value_type,
        std::enable_if_t<std::is_same_v<T, std::vector<Value_t>>, bool> = true>
    T unbox(jl_value_t* value)
    {
        if (not jl_is_array(value))
            assert(false);

        auto* as_array = (jl_array_t*) value;

        std::vector<Value_t> out;
        out.reserve(as_array->length);

        for (size_t i = 0; i < as_array->length; ++i)
            out.push_back(unbox<Value_t>(jl_arrayref(as_array, i)));

        return out;
    }

    /// @brief unbox to array
    template<typename T,
        typename Value_t = typename T::value_type,
        size_t N = sizeof(T) / sizeof(Value_t),
        std::enable_if_t<std::is_same_v<T, std::array<Value_t, N>>, bool> = true>
    T unbox(jl_value_t* value)
    {
        if (not jl_is_array(value))
            assert(false);

        auto* as_array = (jl_array_t*) value;

        if (as_array->length != N)
        {
            std::stringstream str;
            str << "[C++] Trying to unbox julia array of size " << as_array->length << " into C++ array of size " << N << std::endl;
            throw std::length_error(str.str().c_str());
        }

        std::array<Value_t, N> out;
        for (size_t i = 0; i < N; ++i)
            out.at(i) = unbox<Value_t>(jl_arrayref(as_array, i));

        return out;
    }
}