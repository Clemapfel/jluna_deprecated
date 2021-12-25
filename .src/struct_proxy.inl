// 
// Copyright 2021 Clemens Cords
// Created on 24.12.21 by clem (mail@clemens-cords.com)
//

#include <struct_proxy.hpp>

namespace jlwrap
{
    MutableStruct::MutableStruct(jl_value_t* instance)
        : Proxy<State>(instance)
    {
        auto* type = jl_typeof(instance);
        for (size_t i = 0; i < jl_nfields(instance); ++i)
            _fieldname_to_index.insert({jl_symbol_name(jl_field_name(type, i)), i});
    }

    auto MutableStruct::operator[](std::string name) const
    {
        return MutableStruct::FieldProxy(*this, _fieldname_to_index.at(name));
    }
}