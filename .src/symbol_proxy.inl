// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#include <julia.h>
#include <symbol_proxy.hpp>

namespace jlwrap
{
    Symbol::Symbol(const std::string& str)
        : Proxy<State>([](const std::string& str) -> jl_value_t*
                       {
                            std::stringstream command;
                            command << "return Symbol(\"" << str << "\")" << std::endl;
                            return jl_eval_string(command.str().c_str());
                       }(str))
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