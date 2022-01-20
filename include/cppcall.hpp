// 
// Copyright 2022 Clemens Cords
// Created on 20.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <state.hpp>

namespace jluna::cppcall
{
    std::unordered_map<std::string, std::function<jl_value_t*(jl_value_t*)>> _functions;

    jl_value_t* get_nth_element(jl_value_t* tuple, size_t n)
    {
        static jl_function_t* get = jl_get_function(jl_base_module, "get");
        return jl_call3(get, tuple, jl_box_uint64(n + 1), jl_undef_initializer());
    }


    template<typename T, typename Return_t, typename... Args_t>
    concept LambdaType = requires(T lambda)
    {
        std::is_invocable<T, Args_t...>::value;
        std::is_same_v<typename std::invoke_result<T, Args_t...>::type, void>;
    };

    /// @brief check if lambda has specified return and value type
    /// @tparam Lambda_t: lambda type
    /// @tparam Return_t: return type
    /// @tparam Args_t...: argument types

    // lambda: void()
    template<typename Lambda_t,
        std::enable_if_t<
            std::is_invocable<Lambda_t>::value,
            Bool> = true,
        std::enable_if_t<
            std::is_same_v<typename std::invoke_result<Lambda_t, jl_value_t*>::type, void>,
            Bool> = true>
    void register_function(const std::string& name, Lambda_t lambda)
    {
        THROW_IF_UNINITIALIZED;

        _functions.insert({name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            lambda();
            return jl_nothing;
        }});
    }

    // lambda: jl_value_t*()
    template<typename Lambda_t,
        std::enable_if_t<
            std::is_invocable<Lambda_t>::value,
            Bool> = true,
        std::enable_if_t<
            std::is_same_v<typename std::invoke_result<Lambda_t, jl_value_t*>::type, jl_value_t*>,
            Bool> = true>
    void register_function(const std::string& name, Lambda_t lambda)
    {
        THROW_IF_UNINITIALIZED;

        _functions.insert({name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            auto* res = lambda();
            return reinterpret_cast<jl_value_t*>(res);
        }});
    }

    // lambda: jl_value_t*(jl_value_t*)
     template<typename Lambda_t,
        std::enable_if_t<
            std::is_invocable<Lambda_t, jl_value_t*>::value,
            Bool> = true,
        std::enable_if_t<
            std::is_same_v<typename std::invoke_result<Lambda_t, jl_value_t*>::type, jl_value_t*>,
            Bool> = true>
    void register_function(const std::string& name, Lambda_t lambda)
    {
        THROW_IF_UNINITIALIZED;

        _functions.insert({name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return lambda(get_nth_element(tuple, 0));
        }});
    }
}