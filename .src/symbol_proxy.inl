// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#include <julia.h>
#include <symbol_proxy.hpp>

namespace jluna
{
    inline jl_value_t* box(Symbol value)
    {
        return value.operator jl_value_t *();
    }

    inline jl_value_t* box(Symbol& value)
    {
        return value.operator jl_value_t *();
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, jluna::Symbol>, bool> = true>
    T unbox(jl_value_t* value)
    {
        return Symbol(value);
    }

    Symbol::Symbol(const std::string& str)
        : Proxy<State>((jl_value_t*) jl_symbol(str.c_str()), (jl_value_t*) jl_main_module, jl_symbol(str.c_str()))
    {
        THROW_IF_UNINITIALIZED;
    }

    Symbol::Symbol(jl_value_t* value, jl_value_t* owner, jl_sym_t* symbol)
        : Proxy<State>(value, owner, symbol)
    {
        THROW_IF_UNINITIALIZED;
        assert(jl_isa(value, (jl_value_t*) jl_symbol_type) && "value being bound is not a symbol");
    }

    Symbol::operator std::string() const
    {
        return std::string(jl_symbol_name((jl_sym_t*) _value));
    }
}