// 
// Copyright 2022 Clemens Cords
// Created on 03.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <string>
#include <proxy.hpp>
#include <state.hpp>

namespace jluna
{
    /// @brief proxy for julia-side singleton instances of Base.Type{T}
    class Type : public Proxy<State>
    {
        public:
            /// @brief ctor from already existing type
            Type(jl_value_t*);

            /// @brief ctor by lookup in state
            Type(const std::string& type_name);

            /// @brief implicitly decay to julia c-type
            operator jl_datatype_t*();

            /// @brief cast to string
            explicit operator std::string() const;

            /// @brief compare
            /// @param other
            /// @returns true if in julia (==)(*this, other) would return true
            bool operator==(const Type&) const;

            /// @brief compare
            /// @param other
            /// @returns true if in julia (!=)(*this, other) would return true
            bool operator!=(const Type&) const;

            /// @brief is mutable
            /// @returns true if Base.ismutable would return true
            bool is_mutable_type() const;

        private:
            using Proxy<State>::_value;
    };

    /// @brief get type of proxy as jluna::Type
    /// @param proxy
    /// @returns Type
    extern Type get_typeof(Proxy<State>&);
}

#include ".src/type_proxy.inl"