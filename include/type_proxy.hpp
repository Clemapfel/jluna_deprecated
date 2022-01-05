// 
// Copyright 2022 Clemens Cords
// Created on 03.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <string>

namespace jluna
{
    /// @brief proxy for julia-side singleton instances of Base.Type{T}
    class Type
    {
        public:
            /// @brief ctor from already existing type
            Type(jl_value_t*);

            /// @brief decay to julia c-type
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

        private:
            jl_value_t* _singleton;
    };
}

#include ".src/type_proxy.inl"