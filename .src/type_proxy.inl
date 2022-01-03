// 
// Copyright 2022 Clemens Cords
// Created on 03.01.22 by clem (mail@clemens-cords.com)
//

#include <type_proxy.hpp>

namespace jlwrap
{
    Type::Type(jl_value_t* v)
        : _singleton(v)
    {
        assert(jl_isa(v, (jl_value_t*) jl_datatype_type) && "value is not a datatype");
    }

    Type::operator std::string()
    {
        static jl_function_t* to_string = jl_get_function(jl_base_module, "string");
        return std::string(jl_string_data(jl_call1(to_string, _singleton)));
    }

    bool Type::operator==(const Type& other) const
    {
        static jl_function_t* equals = jl_get_function(jl_base_module, "==");
        return jl_unbox_bool(jl_call2(equals, this->_singleton, other._singleton));
    }

    bool Type::operator!=(const Type& other) const
    {
        static jl_function_t* not_equals = jl_get_function(jl_base_module, "!=");
        return jl_unbox_bool(jl_call2(not_equals, this->_singleton, other._singleton));
    }
}