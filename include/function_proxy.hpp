// 
// Copyright 2021 Clemens Cords
// Created on 19.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <proxy.hpp>

namespace jlwrap
{
    class FunctionProxy : public Proxy<jl_function_t>
    {
        public:


        private:
            using Proxy::_value;
    };
}