// 
// Copyright 2022 Clemens Cords
// Created on 08.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <proxy.hpp>
#include <state.hpp>
#include <type_proxy.hpp>
#include <symbol_proxy.hpp>
#include <array_proxy.hpp>

namespace jluna
{
    /// @brief singleton proxy of main module
    static inline Proxy<State> Main = Proxy<State>(nullptr,(jl_sym_t*) nullptr);

    /// @brief singleton proxy of base module
    static inline Proxy<State> Base = Proxy<State>(nullptr, (jl_sym_t*) nullptr);

    /// @brief singleton proxy of base module
    static inline Proxy<State> Core = Proxy<State>(nullptr, (jl_sym_t*) nullptr);
}

