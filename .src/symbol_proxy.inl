// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#include <julia.h>
#include <symbol_proxy.hpp>

namespace jlwrap
{
    inline jl_value_t* box(Symbol value)
    {
        return value.operator jl_value_t *();
    }

    inline jl_value_t* box(Symbol& value)
    {
        return value.operator jl_value_t *();
    }

    Symbol::Symbol(const std::string& str)
        : Proxy<State>((jl_value_t*) jl_symbol(str.c_str()))
    {}

    Symbol::Symbol(jl_value_t* value)
        : Proxy<State>(value)
    {
        assert(jl_isa(value, (jl_value_t*) jl_symbol_type) && "value being bound is not a symbol");
    }

    Symbol::operator std::string()
    {
        return std::string(jl_symbol_name((jl_sym_t*) _value));
    }
}