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
#include <exceptions.hpp>

namespace jluna
{

    jl_value_t* unbox(jl_value_t* value)
    {
        return value;
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_bool_type);
        return jl_unbox_bool(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, char>, bool>>
    T unbox(jl_value_t* value)
    {
        static jl_function_t* convert = jl_get_function(jl_main_module, "convert");

        if (jl_isa(value, (jl_value_t*) jl_uint8_type))
                return char(jl_unbox_uint8(value));
        else
            return char(jl_unbox_uint8(safe_call(convert, (jl_value_t*) jl_uint8_type, value)));
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, int8_t>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_int8_type);
        return jl_unbox_int8(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, int16_t>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_int16_type);
        return jl_unbox_int16(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, int32_t>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_int32_type);
        return jl_unbox_int32(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, int64_t>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_int64_type);
        return jl_unbox_int64(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint8_t>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_uint8_type);
        return jl_unbox_uint8(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint16_t>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_uint16_type);
        return jl_unbox_uint16(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint32_t>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_uint32_type);
        return jl_unbox_uint32(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_uint64_type);
        return jl_unbox_uint64(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, float>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_float32_type);
        return jl_unbox_float32(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, double>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, jl_float64_type);
        return jl_unbox_float64(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, bool>>
    T unbox(jl_value_t* value)
    {
        if (jl_isa(value, (jl_value_t*) jl_string_type))
            return std::string(jl_string_data(value));

        static jl_function_t* to_string = jl_get_function(jl_main_module, "string");
        return std::string(jl_string_data(safe_call(to_string, value)));
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, const char*>, bool>>
    T unbox(jl_value_t* value)
    {
        if (jl_isa(value, (jl_value_t*) jl_string_type))
            return jl_string_data(value);

        static jl_function_t* to_string = jl_get_function(jl_main_module, "string");
        return jl_string_data(safe_call(to_string, value));
    }

    template<typename T, typename S, std::enable_if_t<std::is_same_v<T, std::complex<S>>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, "Complex");

        auto* re = jl_get_nth_field(value, 0);
        auto* im = jl_get_nth_field(value, 1);

        return std::complex<S>(unbox<S>(re), unbox<S>(im));
    }

    template<typename T, typename T1, typename T2, std::enable_if_t<std::is_same_v<T, std::pair<T1, T2>>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, "Pair");

        auto* first = jl_get_nth_field(value, 0);
        auto* second = jl_get_nth_field(value, 1);

        return std::pair<T1, T2>(unbox<T1>(first), unbox<T2>(second));
    }

    template<typename T, typename U, std::enable_if_t<std::is_same_v<T, std::vector<U>>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, "Vector");

        std::vector<U> out;
        out.reserve(jl_array_len(value));

        for (size_t i = 0; i < jl_array_len(value); ++i)
            out.push_back(unbox<U>(jl_arrayref((jl_array_t*) value, i)));

        return out;
    }

    template<typename T, size_t N, typename U, std::enable_if_t<std::is_same_v<T, std::array<U, N>>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, "Array{1}");

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
            auto* v = safe_call(tuple_at, value, jl_box_uint64(i + 1));
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

    template<IsTuple T, std::enable_if_t<std::tuple_size<T>::value != 2, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, "Tuple");

        return detail::unbox_tuple_pre(value, T());
    }
    template<IsDict T, typename Key_t, typename Value_t>
    T unbox(jl_value_t* value)
    {
        assert_type(value, "AbstractDict");

        static jl_function_t* serialize = jl_get_function((jl_module_t*) jl_eval_string("return jluna"), "serialize");

        jl_array_t* as_array = (jl_array_t*) safe_call(serialize, value);

        T out;
        for (size_t i = 0; i < jl_array_len(as_array); ++i)
            out.insert(unbox<std::pair<Key_t, Value_t>>(jl_arrayref(as_array, i)));

        return out;
    }
    template<typename T, typename U, std::enable_if_t<std::is_same_v<T, std::set<U>>, bool>>
    T unbox(jl_value_t* value)
    {
        value = try_convert(value, "Set");

        static jl_function_t* serialize = jl_get_function((jl_module_t*) jl_eval_string("return jluna"), "serialize");
        jl_array_t* as_array = (jl_array_t*) safe_call(serialize, value);

        std::set<U> out;
        for (size_t i = 0; i < jl_array_len(as_array); ++i)
            out.insert(unbox<U>(jl_arrayref(as_array, i)));

        return out;
    }
}