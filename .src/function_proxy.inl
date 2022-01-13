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
            return value();
        }

        FunctionProxy::FunctionProxy(jl_function_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t* symbol)
            : Proxy<State>((jl_value_t*) value, owner, symbol)
        {}

        FunctionProxy::FunctionProxy(jl_function_t* value, jl_sym_t* symbol)
            : Proxy<State>((jl_value_t*) value, symbol)
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
        return SafeFunction(value, nullptr);
    }

    inline jl_value_t* box(Function value)
    {
        return value.operator jl_value_t *();
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, jluna::Function>, bool> = true>
    T unbox(jl_value_t* value)
    {
        return Function(value, nullptr);
    }

    SafeFunction::SafeFunction(jl_function_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t* symbol)
        : detail::FunctionProxy(value, owner, symbol)
    {
        THROW_IF_UNINITIALIZED;

        assert(jl_isa(value, (jl_value_t*) jl_function_type) && "value being bound is not a function");
    }

    SafeFunction::SafeFunction(jl_function_t* value, jl_sym_t* symbol)
        : detail::FunctionProxy(value, symbol)
    {
        THROW_IF_UNINITIALIZED;

        assert(jl_isa(value, (jl_value_t*) jl_function_type) && "value being bound is not a function");
    }

    template<Boxable... Args_t>
    auto SafeFunction::operator()(Args_t&&... args)
    {
        return Proxy<State>(State::safe_call((jl_function_t*) value(), std::forward<Args_t>(args)...), nullptr);
    }

    Function::Function(jl_function_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t* symbol)
        : detail::FunctionProxy(value, owner, symbol)
    {
        THROW_IF_UNINITIALIZED;

        assert(jl_isa(value, (jl_value_t*) jl_function_type) && "value being bound is not a function");
    }

    Function::Function(jl_function_t* value, jl_sym_t* symbol)
        : detail::FunctionProxy(value, symbol)
    {
        THROW_IF_UNINITIALIZED;

        assert(jl_isa(value, (jl_value_t*) jl_function_type) && "value being bound is not a function");
    }

    template<Boxable... Args_t>
    auto Function::operator()(Args_t&&... args)
    {
        return Proxy<State>(State::call((jl_function_t*) value(), std::forward<Args_t>(args)...), nullptr);
    }

}