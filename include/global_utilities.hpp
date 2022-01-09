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
    static inline Proxy<State> Main = Proxy<State>(nullptr, nullptr, (jl_sym_t*) nullptr);

    /// @brief singleton proxy of base module
    static inline Proxy<State> Base = Proxy<State>(nullptr, nullptr, (jl_sym_t*) nullptr);

    /// @brief singleton proxy of base module
    static inline Proxy<State> Core = Proxy<State>(nullptr, nullptr, (jl_sym_t*) nullptr);

    /// @brief get type of proxy
    /// @param proxy
    /// @returns type as Type Proxy
    extern Type type_of(Proxy<State>&);

    /// @brief check if the type of a proxy inherits from type
    /// @param proxy
    /// @param type
    /// @returns result of julia-side isa(proxy, type) call
    extern bool isa(Proxy<State>&, Type);

    /// @brief call julia Base.string on value
    /// @param value
    /// @returns string
    extern std::string to_string(jl_value_t*);
}

