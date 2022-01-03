// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#include <module_proxy.hpp>
#include <symbol_proxy.hpp>

namespace jlwrap
{
    inline jl_value_t* box(Module value)
    {
        return value.operator jl_value_t*();
    }

    inline jl_value_t* box(Module& value)
    {
        return value.operator jl_value_t*();
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, jlwrap::Module>, bool> = true>
    inline T unbox(jl_value_t* value)
    {
        return Module(value);
    }

    Module::Module(jl_value_t* value)
        : Proxy<State>(value)
    {
        assert(jl_isa(value, (jl_value_t*) jl_module_type));
        _eval = jl_get_function(jl_base_module, "eval");
    }

    auto Module::operator[](const std::string& name)
    {
        return State::safe_call(_eval, _value, Symbol(name));
    }

    Module::operator jl_module_t*()
    {
        return (jl_module_t*) _value;
    }
}