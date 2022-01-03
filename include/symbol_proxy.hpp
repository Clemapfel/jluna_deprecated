// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <state.hpp>
#include <proxy.hpp>

namespace jlwrap
{
    class Symbol : public Proxy<State>
    {
        public:
            Symbol(const std::string&);
            Symbol(jl_value_t*);

            explicit operator std::string() override;

        private:
            using Proxy<State>::_value;
    };
}

#include ".src/symbol_proxy.inl"