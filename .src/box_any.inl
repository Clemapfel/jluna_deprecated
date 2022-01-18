


//

#pragma once

#include <box_any.hpp>
#include <.src/common.hpp>

namespace jluna
{
    
    jl_value_t* box(jl_value_t* value)
    {
        return value;
    }

    template<Decayable T>
    jl_value_t* box(T&& value)
    {
        return (jl_value_t*) (value);
    }

    
    jl_value_t* box(const std::string& value)
    {
        std::string command = "return \"" + value + "\"";
        return jl_eval_string(command.c_str());
    }

    
    jl_value_t* box(const char* value)
    {
        std::string command = "return \"" + std::string(value) + "\"";
        return jl_eval_string(command.c_str());
    }

    
    jl_value_t* box(bool value)
    {
        return jl_box_bool(value);
    }

    
    jl_value_t* box(char value)
    {
        return jl_box_char(value);
    }

    
    jl_value_t* box(int8_t value)
    {
        return jl_box_int8(value);
    }

    
    jl_value_t* box(int16_t value)
    {
        return jl_box_int16(value);
    }

    
    jl_value_t* box(int32_t value)
    {
        return jl_box_int32(value);
    }

    
    jl_value_t* box(int64_t value)
    {
        return jl_box_int64(value);
    }

    
    jl_value_t* box(uint8_t value)
    {
        return jl_box_uint8(value);
    }

    
    jl_value_t* box(uint16_t value)
    {
        return jl_box_uint16(value);
    }

    
    jl_value_t* box(uint32_t value)
    {
        return jl_box_uint32(value);
    }

    
    jl_value_t* box(uint64_t value)
    {
        return jl_box_uint64(value);
    }

    
    jl_value_t* box(float value)
    {
        return jl_box_float32(value);
    }

    
    jl_value_t* box(double value)
    {
        return jl_box_float64(value);
    }

    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool>>
    jl_value_t* box(std::complex<T> value)
    {
        static jl_function_t* complex = jl_get_function(jl_base_module, "complex");
        return jl_call2(complex, box(value.real()), box(value.imag()));
    }

    template<typename T>
    jl_value_t* box(const std::vector<T>& value)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("jluna");
        static jl_function_t* vector = jl_get_function(jluna_module, "make_vector");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());
        for (auto& v : value)
            args.push_back(box(v));

        return jl_call(vector, args.data(), args.size());
    }

    template<typename T1, typename T2>
    jl_value_t* box(const std::pair<T1, T2>& value)
    {
        static jl_function_t* pair = jl_get_function(jl_core_module, "Pair");
        return jl_call2(pair, box(value.first), box(value.second));
    }

    template<typename... Ts>
    jl_value_t* box(const std::tuple<Ts...>& value)
    {
        static jl_function_t* tuple = jl_get_function(jl_core_module, "tuple");

        std::vector<jl_value_t*> args;
        args.reserve(sizeof...(Ts));

        std::apply([&](auto&&... elements) {
            (args.push_back(box(elements)), ...);
        }, value);

        return jl_call(tuple, args.data(), args.size());
    }

    template<typename Key_t, typename Value_t>
    jl_value_t* box(const std::map<Key_t, Value_t>& value)
    {
        static jl_function_t* iddict = jl_get_function(jl_base_module, "IdDict");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());

        for (const std::pair<Key_t, Value_t>& pair : value)
            args.push_back(box(pair));

        return jl_call(iddict, args.data(), args.size());
    }
    template<typename Key_t, typename Value_t>
    jl_value_t* box(const std::unordered_map<Key_t, Value_t>& value)
    {
        static jl_function_t* dict = jl_get_function(jl_base_module, "Dict");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());

        for (const std::pair<Key_t, Value_t>& pair : value)
            args.push_back(box<std::pair<Key_t, Value_t>>(pair));

        return jl_call(dict, args.data(), args.size());
    }

    template<typename T>
    jl_value_t* box(const std::set<T>& value)
    {
        static jl_function_t* make_set = jl_get_function((jl_module_t*) jl_eval_string("return jluna"), "make_set");

        std::vector<jl_value_t*> args;
        args.reserve(value.size());

        for (const auto& t : value)
            args.push_back(box(t));

        return jl_call(make_set, args.data(), args.size());
    }

    template<typename Return_t, CastableTo<Return_t> Arg_t>
    jl_value_t* box(Arg_t t)
    {
        return box(static_cast<Return_t>(t));
    }
}