// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

namespace jlwrap
{
    class Proxy
    {
        public:
            Proxy(jl_sym_t* symbol, jl_value_t* value, jl_datatype_t* type);
            ~Proxy();

            Proxy(const Proxy&);
            Proxy& operator=(const Proxy&);

            Proxy(Proxy&&) noexcept;
            Proxy& operator=(Proxy&&) noexcept;

        protected:
            jl_datatype_t* _type;
            jl_sym_t* _symbol;
            jl_value_t* _value;
    };
}

#include ".src/proxy.inl"