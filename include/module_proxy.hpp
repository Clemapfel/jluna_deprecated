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
            /// @brief CTOR: attach julia-side module
            /// @param value
            Module(jl_value_t*);

            /// @brief access field/function in module
            /// @param name
            /// @returns field or function
            /// @exceptions: UndefVarError if the field/function does not exist
            auto operator[](const std::string&);

            /// @brief decay to julia C module type
            operator jl_module_t*();

        private:
            static inline jl_function_t* _eval = nullptr;
    };
}

#include ".src/module_proxy.inl"