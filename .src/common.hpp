// 
// Copyright 2022 Clemens Cords
// Created on 14.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace jluna
{
    /// concept: can From be static cast to To
    template<typename From, typename To>
    concept CastableTo = requires(From t)
    {
        {static_cast<To>(t)};
    };

    /// concept: iterable range
    template<typename T>
    concept Iterable = requires(T t)
    {
        {t.begin()};
        {t.end()};
        typename T::value_type;
    };

    // concept: has value type
    template<typename T>
    concept HasValueType = requires(T t)
    {
        typename T::value_type;
    };

    // concept: is map
    template<typename T>
    concept IsDict = requires(T t)
    {
        typename T::key_type;
        typename T::mapped_type;
        std::is_same_v<typename T::value_type, std::pair<typename T::key_type, typename T::mapped_type>>;
    };

    // concept: is tuple but not pair
    template<typename T>
    concept IsTuple = requires(T t)
    {
        std::tuple_size<T>::value;
    };

    // concept: has ctor that takes jl_value_t* and symbol
    template<typename T>
    concept IsUnnamedProxy = requires(T t, jl_value_t* v)
    {
        T(v, (jl_sym_t*) nullptr);
    };

    /// @brief concept that describes lambda with signature T(Args_t...)
    template<typename T, typename... Args_t>
    concept LambdaType =
    requires(T lambda)
    {
        std::is_invocable<T, Args_t...>::value;
        typename std::invoke_result<T, Args_t...>::type;
    };

    /// get sizeof array
    /// reference: https://brevzin.github.io/c++/2020/02/05/constexpr-array-size/
    template<HasValueType Array_t>
    struct array_size
    {
        static inline constexpr size_t value = sizeof(Array_t) / sizeof(typename Array_t::value_type);
    };

    // call any function with exception forwarding
    template<typename... Args_t>
    static auto safe_call(jl_function_t* function, Args_t... args)
    {
        THROW_IF_UNINITIALIZED;

        static jl_function_t* tostring = jl_get_function(jl_base_module, "string");

        std::vector<jl_value_t*> params = {(jl_value_t*) function};
        (params.push_back(reinterpret_cast<jl_value_t*>(args)), ...);

        static jl_function_t* safe_call = jl_get_function((jl_module_t*) jl_eval_string("return jluna.exception_handler"), "safe_call");
        auto* result = jl_call(safe_call, params.data(), params.size());

        forward_last_exception();
        return result;
    }

    static auto safe_call_params(std::vector<jl_value_t*> params)
    {
        THROW_IF_UNINITIALIZED;
        static jl_function_t* tostring = jl_get_function(jl_base_module, "string");

        static jl_function_t* safe_call = jl_get_function((jl_module_t*) jl_eval_string("return jluna.exception_handler"), "safe_call");
        auto* result = jl_call(safe_call, params.data(), params.size());

        forward_last_exception();
        return result;
    }

    // call any function without exception forwarding
    template<typename... Args_t>
    static auto call(jl_function_t* function, Args_t... args)
    {
        THROW_IF_UNINITIALIZED;

        static jl_function_t* tostring = jl_get_function(jl_base_module, "string");

        auto params = std::vector<jl_value_t*>();
        (params.push_back(reinterpret_cast<jl_value_t*>(args)), ...);

        return jl_call(function, params.data(), params.size());
    }

    static jl_function_t* get_function(const std::string& module_name, const std::string& function_name)
    {
        return jl_get_function((jl_module_t*) jl_eval_string(("return " + module_name).c_str()), function_name.c_str());
    }

    static void assert_type(jl_value_t* value, const std::string& type_str)
    {
        static jl_function_t* assert_isa = get_function("jluna", "assert_isa");
        safe_call(assert_isa, value, jl_symbol(type_str.c_str()));
    }

    static jl_value_t* try_convert(jl_value_t* origin, const std::string& target_type)
    {
        static jl_function_t* convert = get_function("jluna", "convert");
        return safe_call(convert, origin, jl_symbol(target_type.c_str()));
    }

    static jl_value_t* try_convert(jl_value_t* origin, jl_datatype_t* type)
    {
        static jl_function_t* convert = jl_get_function(jl_base_module, "convert");
        return safe_call(convert, type, origin);
    }
}

