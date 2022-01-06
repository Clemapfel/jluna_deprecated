// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <state.hpp>
#include <proxy.hpp>

namespace jluna
{
    /// @brief symbol wrapper
    class Symbol : public Proxy<State>
    {
        public:
            /// @brief allocate new symbol julia-side from string
            /// @param string
            Symbol(const std::string&);

            /// @brief bind already existing symbol
            /// @param value
            Symbol(jl_value_t*);

            /// @brief convert to string
            /// @returns string
            explicit operator std::string() const override;

        private:
            using Proxy<State>::_value;
    };
}

#include ".src/symbol_proxy.inl"