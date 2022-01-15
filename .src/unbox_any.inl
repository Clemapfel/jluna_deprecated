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
    jl_value_t* unbox(jl_value_t* value)
    {
        return value;
    }

    /// @brief unbox to bool
    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_bool_type))
                return jl_unbox_bool(value);
        else
            return jl_unbox_bool(jl_call2(convert, (jl_value_t*) jl_bool_type, value));
    }

    /// @brief unbox to char
    template<typename T, std::enable_if_t<std::is_same_v<T, char>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_bool_type))
                return jl_unbox_int8(value);
        else
            return jl_unbox_int8(jl_call2(convert, (jl_value_t*) jl_char_type, value));
    }

    /// @brief unbox to int8
    template<typename T, std::enable_if_t<std::is_same_v<T, int8_t>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_int8_type))
                return jl_unbox_int8(value);
        else
            return jl_unbox_int8(jl_call2(convert, (jl_value_t*) jl_int8_type, value));
    }

    /// @brief unbox to int16
    template<typename T, std::enable_if_t<std::is_same_v<T, int16_t>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_int16_type))
                return jl_unbox_int16(value);
        else
            return jl_unbox_int16(jl_call2(convert, (jl_value_t*) jl_int16_type, value));
    }

    /// @brief unbox to int32
    template<typename T, std::enable_if_t<std::is_same_v<T, int32_t>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_int32_type))
                return jl_unbox_int32(value);
        else
            return jl_unbox_int32(jl_call2(convert, (jl_value_t*) jl_int32_type, value));
    }

    /// @brief unbox to int64
    template<typename T, std::enable_if_t<std::is_same_v<T, int64_t>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_int64_type))
                return jl_unbox_int64(value);
        else
            return jl_unbox_int64(jl_call2(convert, (jl_value_t*) jl_int64_type, value));
    }

    /// @brief unbox to uint8
    template<typename T, std::enable_if_t<std::is_same_v<T, uint8_t>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint8_type))
                return jl_unbox_uint8(value);
        else
            return jl_unbox_uint8(jl_call2(convert, (jl_value_t*) jl_uint8_type, value));
    }

    /// @brief unbox to uint16
    template<typename T, std::enable_if_t<std::is_same_v<T, uint16_t>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint16_type))
                return jl_unbox_uint16(value);
        else
            return jl_unbox_uint16(jl_call2(convert, (jl_value_t*) jl_uint16_type, value));
    }

    /// @brief unbox to uint32
    template<typename T, std::enable_if_t<std::is_same_v<T, uint32_t>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint32_type))
                return jl_unbox_uint32(value);
        else
            return jl_unbox_uint32(jl_call2(convert, (jl_value_t*) jl_uint32_type, value));
    }

    /// @brief unbox to uint64
    template<typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint64_type))
                return jl_unbox_uint64(value);
        else
            return jl_unbox_uint64(jl_call2(convert, (jl_value_t*) jl_int64_type, value));
    }

    /// @brief unbox to float16 & float32
    template<typename T, std::enable_if_t<std::is_same_v<T, float>, bool>>
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
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_float64_type))
                return jl_unbox_float64(value);
        else
            return jl_unbox_float64(jl_call2(convert, (jl_value_t*) jl_float64_type, value));
    }

    /// @brief unbox to string
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, bool>>
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
    template<typename T, typename S, std::enable_if_t<std::is_same_v<T, std::complex<S>>, bool>>
    T unbox(jl_value_t* value)
    {
        assert(jl_isa(value, jl_eval_string("return Complex")));

        auto* re = jl_get_nth_field(value, 0);
        auto* im = jl_get_nth_field(value, 1);

        return std::complex<S>(unbox<S>(re), unbox<S>(im));
    }

    /// @brief unbox to pair
    template<typename T, typename T1, typename T2, std::enable_if_t<std::is_same_v<T, std::pair<T1, T2>>, bool>>
    T unbox(jl_value_t* value)
    {
        assert(jl_isa(value, (jl_value_t*) jl_pair_type));

        auto* first = jl_get_nth_field(value, 0);
        auto* second = jl_get_nth_field(value, 1);

        return std::pair<T1, T2>(unbox<T1>(first), unbox<T2>(second));
    }

    /// @brief unbox to vector
    template<typename T, typename U, std::enable_if_t<std::is_same_v<T, std::vector<U>>, bool>>
    T unbox(jl_value_t* value)
    {
        assert(jl_isa(value, (jl_value_t*) jl_array_type));

        std::vector<U> out;
        out.reserve(jl_array_len(value));

        for (size_t i = 0; i < jl_array_len(value); ++i)
            out.push_back(unbox<U>(jl_arrayref((jl_array_t*) value, i)));

        return out;
    }

    /// @brief unbox to array
    template<typename T, size_t N, typename U, std::enable_if_t<std::is_same_v<T, std::array<U, N>>, bool>>
    T unbox(jl_value_t* value)
    {
        assert(jl_isa(value, (jl_value_t*) jl_array_type));
        assert(N == jl_unbox_int64((jl_value_t*) jl_array_len(value)));

        std::array<U, N> out;

        for (size_t i = 0; i < jl_array_len(value); ++i)
            out.at(i) = (unbox<U>(jl_arrayref((jl_array_t*) value, i)));

        return out;
    }

    namespace detail    // helper functions for tuple unboxing
    {
        template<typename Tuple_t, typename Value_t, size_t i>
        void unbox_tuple_aux_aux(Tuple_t& tuple, jl_value_t* value)
        {
            static jl_function_t* tuple_at = (jl_function_t*) jl_eval_string("jluna.tuple_at");
            auto* v = jl_call2(tuple_at, value, jl_box_uint64(i + 1));
            std::get<i>(tuple) = unbox<std::tuple_element_t<i, Tuple_t>>(v);
        }

        template<typename Tuple_t, typename Value_t, std::size_t... is>
        void unbox_tuple_aux(Tuple_t& tuple, jl_value_t* value, std::index_sequence<is...> _)
        {
            (unbox_tuple_aux_aux<Tuple_t, Value_t, is>(tuple, value), ...);
        }

        template<typename... Ts>
        std::tuple<Ts...> unbox_tuple(jl_value_t* value)
        {
            std::tuple<Ts...> out;
            (unbox_tuple_aux<std::tuple<Ts...>, Ts>(out, value, std::index_sequence_for<Ts...>{}), ...);

            return out;
        }

        template<typename... Ts>
        std::tuple<Ts...> unbox_tuple_pre(jl_value_t* v, std::tuple<Ts...>)
        {
            return unbox_tuple<Ts...>(v);
        }
    }

    ///@brief unbox tuple (but not pair)
    template<IsTuple T, std::enable_if_t<std::tuple_size<T>::value != 2, bool>>
    T unbox(jl_value_t* value)
    {
        return detail::unbox_tuple_pre(value, T());
    }

    /// @brief unbox map
    template<IsDict T, typename Key_t, typename Value_t>
    T unbox(jl_value_t* value)
    {
        // TODO: optimize
        static jl_function_t* serialize = jl_get_function((jl_module_t*) jl_eval_string("return jluna"), "serialize");

        jl_array_t* as_array = (jl_array_t*) jl_call1(serialize, value);

        T out;
        for (size_t i = 0; i < jl_array_len(as_array); ++i)
            out.insert(unbox<std::pair<Key_t, Value_t>>(jl_arrayref(as_array, i)));

        return out;
    }

    /// @brief unbox set
    template<typename T, typename U, std::enable_if_t<std::is_same_v<T, std::set<U>>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* serialize = jl_get_function((jl_module_t*) jl_eval_string("return jluna"), "serialize");
        jl_array_t* as_array = (jl_array_t*) jl_call1(serialize, value);

        std::set<U> out;
        for (size_t i = 0; i < jl_array_len(as_array); ++i)
            out.insert(unbox<U>(jl_arrayref(as_array, i)));

        return out;
    }
}