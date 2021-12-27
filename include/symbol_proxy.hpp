// 
// Copyright 2021 Clemens Cords
// Created on 27.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <state.hpp>

namespace jlwrap
{
    class SymbolProxy : public Proxy<State>
    {
        public:
            /*[[implicit]]*/ SymbolProxy(std::string);

    };
}