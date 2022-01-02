// 
// Copyright 2021 Clemens Cords
// Created on 24.12.21 by clem (mail@clemens-cords.com)
//

#include <struct_proxy.hpp>

namespace jlwrap
{
    auto MutableStruct::operator[](const std::string& field_name)
    {
        return Proxy<State>::operator[](field_name);
    }

    template<typename T>
    T MutableStruct::operator[](const std::string& field_name) const
    {
        return Proxy<State>::operator[]<T>(field_name);
    }

    template<typename T>
    T Struct::operator[](const std::string& field_name) const
    {
        return Proxy<State>::operator[]<T>(field_name);
    }
}