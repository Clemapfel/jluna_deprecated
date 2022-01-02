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

            operator std::string();
            operator jl_value_t*();

        private:
            using Proxy<State>::_value;
    };
}