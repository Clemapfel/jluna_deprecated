// 
// Copyright 2021 Clemens Cords
// Created on 24.12.21 by clem (mail@clemens-cords.com)
//

#include <struct_proxy.hpp>

namespace jlwrap
{
    auto MutableStruct::operator[](const std::string& field_name)
    {
        return Proxy<State>::get_field(field_name);
    }

    template<typename T>
    T MutableStruct::operator[](const std::string& field_name) const
    {
        return Proxy<State>::get_field<T>(field_name);
    }

    MutableStruct::MutableStruct(jl_value_t* value)
        : Proxy<State>(value)
    {
        assert(jl_is_structtype(value) && "value being bound is not a struct");
    }

    template<typename T>
    T Struct::operator[](const std::string& field_name) const
    {
        return Proxy<State>::get_field<T>(field_name);
    }

    Struct::Struct(jl_value_t* value)
        : Proxy<State>(value)
    {
        assert(jl_is_structtype(value) && "value being bound is not a struct");
    }
}