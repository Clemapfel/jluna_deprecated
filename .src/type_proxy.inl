// 
// Copyright 2022 Clemens Cords
// Created on 03.01.22 by clem (mail@clemens-cords.com)
//

#include <type_proxy.hpp>

namespace jluna
{
    inline jl_value_t* box(Type type)
    {
        return (jl_value_t*) type.operator _jl_datatype_t *();
    }

    Type::Type(jl_value_t* v, Proxy<State>* owner, jl_sym_t* symbol)
        : Proxy<State>(v, owner, symbol)
    {
        THROW_IF_UNINITIALIZED;
        assert(jl_isa(v, (jl_value_t*) jl_type_type) || jl_isa(v, (jl_value_t*) jl_type_type) && "value is not a type or datatype");
    }

    Type::operator jl_datatype_t*()
    {
        return (jl_datatype_t*) _value;
    }

    Type::operator std::string() const
    {
        static jl_function_t* to_string = jl_get_function(jl_base_module, "string");
        return std::string(jl_string_data(jl_call1(to_string, _value)));
    }

    bool Type::operator==(const Type& other) const
    {
        static jl_function_t* equals = jl_get_function(jl_base_module, "==");
        return jl_unbox_bool(jl_call2(equals, this->_value, other._value));
    }

    bool Type::operator!=(const Type& other) const
    {
        static jl_function_t* not_equals = jl_get_function(jl_base_module, "!=");
        return jl_unbox_bool(jl_call2(not_equals, this->_value, other._value));
    }

    bool Type::is_mutable_type() const
    {
        static jl_function_t* ismutabletype = jl_get_function(jl_base_module, "ismutabletype");
        return jl_call1(ismutabletype, _value);
    }
}