// 
// Copyright 2022 Clemens Cords
// Created on 03.01.22 by clem (mail@clemens-cords.com)
//

#include <type_proxy.hpp>

namespace jlwrap
{
    inline jl_value_t* box(Type type)
    {
        return (jl_value_t*) type.operator _jl_datatype_t *();
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, jlwrap::Type>, bool> = true>
    T unbox(jl_value_t* value)
    {
        return Type(value);
    }

    Type::Type(jl_value_t* v)
        : _singleton(v)
    {
        assert(jl_isa(v, (jl_value_t*) jl_datatype_type) && "value is not a datatype");
    }

    Type::operator _jl_datatype_t*()
    {
        return (jl_datatype_t*) _singleton;
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