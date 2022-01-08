// 
// Copyright 2022 Clemens Cords
// Created on 08.01.22 by clem (mail@clemens-cords.com)
//

#include <global_utilities.hpp>

namespace jluna
{
    Type type_of(Proxy<State>& v)
    {
        return Type(jl_typeof((jl_value_t*) v));
    }

    bool isa(Proxy<State>& v, Type t)
    {
        return (bool) jl_isa((jl_value_t*) v, (jl_value_t*) t);
    }
}