// 
// Copyright 2022 Clemens Cords
// Created on 30.01.22 by clem (mail@clemens-cords.com)
//

#include <box_any.hpp>

namespace jluna
{
    template<Decayable T>
    jl_value_t* box(const T& value)
    {
        return static_cast<jl_value_t*>(value);
    }

    jl_value_t* box(jl_value_t* value)
    {
        return value;
    }

    template<typename Return_t, CastableTo<Return_t> Arg_t>
    jl_value_t* box(Arg_t t)
    {
        return box(static_cast<Return_t>(t));
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, const char*>, bool>>
    jl_value_t* box(T value)
    {
        std::string command = "return \"" + std::string(value) + "\"";
        return jl_eval_string(command.c_str());
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, std::bool_constant<true>>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_bool(value.operator bool());
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, std::bool_constant<false>>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_bool(value.operator bool());
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, char>, bool>>
    jl_value_t* box(T value)
    {
        static jl_function_t* to_char = jl_get_function(jl_base_module, "Char");
        return jl_call1(to_char, jl_box_uint8(value));
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, int8_t>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_int8(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, int16_t>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_int16(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, int32_t>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_int32(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, int64_t>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_int64(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint8_t>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_uint8(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint16_t>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_uint16(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint32_t>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_uint32(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_uint64(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, float>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_float32(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, double>, bool>>
    jl_value_t* box(T value)
    {
        return jl_box_float64(value);
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, bool>>
    jl_value_t* box(const T& value)
    {
        std::string command = "return \"" + value + "\"";
        return jl_eval_string(command.c_str());
    }

    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool>>
    jl_value_t* box(std::complex<T> value)
    {
        static jl_function_t* complex = jl_get_function(jl_base_module, "complex");
        return jl_call2(complex, box(value.real()), box(value.imag()));
    }

    template<typename T, typename U, std::enable_if_t<std::is_same_v<T, std::vector<U>>, bool>>
    jl_value_t* box(T value)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("jluna");
        static jl_function_t* vector = jl_get_function(jluna_module, "make_vector");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());
        for (auto& v : value)
            args.push_back(box(v));

        return jl_call(vector, args.data(), args.size());
    }

    template<typename T, typename T1, typename T2, std::enable_if_t<std::is_same_v<T, std::pair<T1, T2>>, bool> >
    jl_value_t* box(T value)
    {
        static jl_function_t* pair = jl_get_function(jl_core_module, "Pair");
        return jl_call2(pair, box(value.first), box(value.second));
    }

    template<IsTuple T, std::enable_if_t<std::tuple_size<T>::value != 2, bool>>
    jl_value_t* box(T value)
    {
        static jl_function_t* tuple = jl_get_function(jl_core_module, "tuple");

        std::vector<jl_value_t*> args;
        args.reserve(std::tuple_size_v<T>);

        std::apply([&](auto&&... elements) {
            (args.push_back(box(elements)), ...);
        }, value);

        return jl_call(tuple, args.data(), args.size());
    }

    template<typename T, typename Key_t, typename Value_t, std::enable_if_t<std::is_same_v<T, std::map<Key_t, Value_t>>, bool>>
    jl_value_t* box(const T& value)
    {
        static jl_function_t* iddict = jl_get_function(jl_base_module, "IdDict");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());

        for (const std::pair<Key_t, Value_t>& pair : value)
            args.push_back(box(pair));

        return jl_call(iddict, args.data(), args.size());
    }

    template<typename T, typename Key_t, typename Value_t, std::enable_if_t<std::is_same_v<T, std::unordered_map<Key_t, Value_t>>, bool>>
    jl_value_t* box(const T& value)
    {
        static jl_function_t* iddict = jl_get_function(jl_base_module, "Dict");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());

        for (const std::pair<Key_t, Value_t>& pair : value)
            args.push_back(box(pair));

        return jl_call(iddict, args.data(), args.size());
    }

    template<typename T, typename U, std::enable_if_t<std::is_same_v<T, std::set<U>>, bool>>
    jl_value_t* box(const T& value)
    {
        static jl_function_t* make_set = jl_get_function((jl_module_t*) jl_eval_string("return jluna"), "make_set");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());

        for (const auto& t : value)
            args.push_back(box(t));

        return jl_call(make_set, args.data(), args.size());
    }
}