// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#include <function_proxy.hpp>

namespace jluna
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

    inline jl_value_t* box(SafeFunction& value)
    {
        return value.operator jl_value_t *();
    }

    inline jl_value_t* box(SafeFunction value)
    {
        return value.operator jl_value_t *();
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, jluna::SafeFunction>, bool> = true>
    T unbox(jl_value_t* value)
    {
        return SafeFunction(value);
    }

    inline jl_value_t* box(Function& value)
    {
        return value.operator jl_value_t *();
    }

    inline jl_value_t* box(Function value)
    {
        return value.operator jl_value_t *();
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, jluna::Function>, bool> = true>
    T unbox(jl_value_t* value)
    {
        return Function(value);
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