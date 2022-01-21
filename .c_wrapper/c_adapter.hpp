// 
// Copyright 2022 Clemens Cords
// Created on 19.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <stddef.h>

#ifdef __cplusplus

#include <map>
#include <julia.h>
#include <functional>
#include <string>

extern "C"
{
    namespace c_adapter
    {
        /// @brief holds lambda registers via jluna
        static std::map<size_t, std::function<jl_value_t*(jl_value_t*)>> _functions;

        /// @brief initialize c-adapter
        /// @note call from julia using: ccall((:initialize, "./libjluna_c_adapter.so"), Cvoid, ())
        void initialize();

        /// @brief hash lambda-side
        size_t hash(const std::string&);

        /// @brief add lambda to function register
        void register_function(const std::string& name, std::function<jl_value_t*(jl_value_t*)>&&);

        /// @brief call lambda by id
        void call_function(size_t);
    }
}

#else

void initialize();

#endif