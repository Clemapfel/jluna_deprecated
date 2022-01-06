// 
// Copyright 2021 Clemens Cords
// Created on 24.12.21 by clem (mail@clemens-cords.com)
//

#include <struct_proxy.hpp>

namespace jluna
{
    inline jl_value_t* box(MutableStruct& value)
    {
        return value.operator jl_value_t *();
    }

    inline jl_value_t* box(Struct& value)
    {
        return value.operator jl_value_t *();
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, jluna::Struct>, bool> = true>
    T unbox(jl_value_t* value)
    {
        return Struct(value);
    }

    inline jl_value_t* box(MutableStruct value)
    {
        return value.operator jl_value_t *();
    }

    inline jl_value_t* box(Struct value)
    {
        return value.operator jl_value_t *();
    }

    template<typename T, std::enable_if_t<std::is_same_v<T, jluna::MutableStruct>, bool> = true>
    T unbox(jl_value_t* value)
    {
        return MutableStruct(value);
    }

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
        THROW_IF_UNINITIALIZED;
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
        THROW_IF_UNINITIALIZED;
        assert(jl_is_structtype(value) && "value being bound is not a struct");
    }
}