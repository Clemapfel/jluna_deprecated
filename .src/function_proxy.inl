// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#include <function_proxy.hpp>

namespace jlwrap
{
    namespace detail
    {
        FunctionProxy::operator jl_function_t*() noexcept
        {
            return _value;
        }

        FunctionProxy::FunctionProxy(jl_function_t* value)
            : Proxy<State>(value)
        {}
    }

    SafeFunction::SafeFunction(jl_function_t* value)
        : detail::FunctionProxy(value)
    {
        assert(jl_isa(value, (jl_value_t*) jl_function_type) && "value being bound is not a function");
    }

    template<typename... Args_t>
    auto SafeFunction::operator()(Args_t&&... args)
    {
        return Proxy<State>(State::safe_call((jl_function_t*) _value, std::forward<Args_t>(args)...));
    }

    /*
    template<typename... Args_t>
    jl_value_t* SafeFunction::operator()(Args_t&&... args)
    {
        return State::call((jl_function_t*) _value, std::forward<Args_t>(args)...);
    }
     */

    Function::Function(jl_function_t* value)
        : detail::FunctionProxy(value)
    {
        assert(jl_isa(value, (jl_value_t*) jl_function_type) && "value being bound is not a function");
    }

    template<typename... Args_t>
    auto Function::operator()(Args_t&&... args)
    {
        return Proxy<State>(State::call((jl_function_t*) _value, std::forward<Args_t>(args)...));
    }

}