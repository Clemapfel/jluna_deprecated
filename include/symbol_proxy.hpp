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
            /// @brief bind already existing symbol
            /// @param value
            /// @param owner
            /// @param symbol of the variable
            Symbol(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t*);

            /// @brief bind already existing symbol
            /// @param value
            /// @param symbol of the variable
            Symbol(jl_value_t* value, jl_sym_t* = nullptr);

            /// @brief convert to string
            /// @returns string
            explicit operator std::string() const override;

        private:
            using Proxy<State>::_content;
    };
}

#include ".src/symbol_proxy.inl"