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
    {}

    template<typename... Args_t>
    jl_value_t* SafeFunction::operator()(Args_t&&... args)
    {
        return State::call((jl_function_t*) _value, std::forward<Args_t>(args)...);
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
    {}

    template<typename... Args_t>
    jl_value_t* Function::operator()(Args_t... args)
    {
        return State::safe_call((jl_function_t*) _value, args...);
    }

}