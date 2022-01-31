// 
// Copyright 2022 Clemens Cords
// Created on 08.01.22 by clem (mail@clemens-cords.com)
//

#include <global_utilities.hpp>

namespace jluna
{
    Type type_of(Proxy<State>& v)
    {
        THROW_IF_UNINITIALIZED;
        return Type(jl_typeof((jl_value_t*) v));
    }

    bool isa(Proxy<State>& v, Type t)
    {
        THROW_IF_UNINITIALIZED;
        return (bool) jl_isa((jl_value_t*) v, (jl_value_t*) t);
    }

    std::string to_string(jl_value_t* value)
    {
        THROW_IF_UNINITIALIZED;
        static jl_function_t* string = jl_get_function(jl_base_module, "string");
        return std::string(jl_string_data(jl_call1(string, value)));
    }
}