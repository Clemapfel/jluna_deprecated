// 
// Copyright 2021 Clemens Cords
// Created on 19.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <proxy.hpp>

namespace jlwrap
{
    union State;

    class Function : protected Proxy<State>
    {
        public:
            Function() = delete;

            /// @brief bind to function pointer
            Function(jl_function_t*);

            /// @brief implicitly decay to function pointer
            operator jl_function_t*();

            /// @brief call operator
            /// @tparams Args_t: argument types
            /// @param args
            /// @returns determined by context
            template<typename... Args_t>
            decltype(auto) operator()(Args_t...);

        private:
            using Proxy<State>::_value;
    };
}

#include ".src/function_proxy.inl"