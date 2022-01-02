// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <proxy.hpp>
#include <state.hpp>

namespace jlwrap
{
    class Module : public Proxy<State>
    {
        public:
            Module(jl_value_t*);
            auto operator[](const std::string&);

            operator jl_module_t*();

        private:
            static inline jl_function_t* _eval = nullptr;
    };
}

#include ".src/module_proxy.inl"