// 
// Copyright 2022 Clemens Cords
// Created on 20.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <typedefs.hpp>

namespace jluna
{
    namespace detail
    {
        /// @brief concept that describes lambda with signature T(Args_t...)
        template<typename T, typename... Args_t>
        concept LambdaType =
        requires(T lambda)
        {
            std::is_invocable<T, Args_t...>::value;
            typename std::invoke_result<T, Args_t...>::type;
        };

        /// @brief forward lambda returning void as jl_nothing
        /// @param func: lambda
        /// @param args: arguments
        template<typename Lambda_t, typename Return_t, typename... Args_t, std::enable_if_t<std::is_same_v<Return_t, void>, Bool> = true>
        jl_value_t* invoke_lambda(const Lambda_t* func, Args_t... args);

        /// @brief forward lambda returning jl_value_t* as jl_value_t*
        /// @param func: lambda
        /// @param args: arguments
        template<typename Lambda_t, typename Return_t, typename... Args_t, std::enable_if_t<std::is_same_v<Return_t, jl_value_t*>, Bool> = true>
        jl_value_t* invoke_lambda(const Lambda_t* func, Args_t... args);
    }

    /// @brief register lambda with signature void() or jl_value_t*()
    /// @param name: function name
    /// @param lambda
    template<detail::LambdaType<> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda);

    /// @brief register lambda with signature void(jl_value_t*) or jl_value_t*(jl_value_t*)
    /// @param name: function name
    /// @param lambda
    template<detail::LambdaType<jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda);

    /// @brief register lambda with signature void(jl_value_t*, jl_value_t*) or jl_value_t*(jl_value_t*, jl_value_t*)
    /// @param name: function name
    /// @param lambda
    template<detail::LambdaType<jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda);

    /// @brief register lambda with signature void(3x jl_value_t*) or jl_value_t*(3x jl_value_t*)
    /// @param name: function name
    /// @param lambda
    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda);

    /// @brief register lambda with signature void(4x jl_value_t*) or jl_value_t*(4x jl_value_t*)
    /// @param name: function name
    /// @param lambda
    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda);

    /// @brief register lambda with signature void(5x jl_value_t*) or jl_value_t*(5x jl_value_t*)
    /// @param name: function name
    /// @param lambda
    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda);

    /// @brief register lambda with signature void(std::vector<jl_value_t*>) or jl_value_t*(std::vector<jl_value_t*>)
    /// @param name: function name
    /// @param lambda
    template<detail::LambdaType<std::vector<jl_value_t*>> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda);


    template<detail::LambdaType<> Lambda_t>
    void assign_function(const std::string& name, Lambda_t&& lambda);

    template<detail::LambdaType<jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda);
}

#include ".src/cppcall.inl"